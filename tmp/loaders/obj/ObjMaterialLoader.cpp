#include "erm/resources/loaders/obj/ObjMaterialLoader.h"

#include "erm/resources/ResourcesManager.h"

#include "erm/resources/materials/Material.h"

#include "erm/utils/Utils.h"

#include <fstream>
#include <iostream>
#include <optional>

namespace erm {

bool ParseMaterialsLib(
	std::mutex& mutex,
	std::atomic<bool>& stop,
	const char* path,
	ResourcesManager& resourcesManager)
{
	std::ifstream stream(path);

	if (!stream.is_open())
	{
		std::cout << "No such file: " << path << std::endl;
		return false;
	}

	std::string line;

	std::optional<Material> mat;
	bool skip = false;

	while (std::getline(stream, line))
	{
		if (stop)
		{
			stream.close();
			return false;
		}

		const std::vector<std::string> splitted = Utils::SplitString(line, ' ');

		if (splitted.size() > 0)
		{
			if (splitted[0].compare("newmtl") == 0)
			{
				if (!skip && mat)
				{
					mutex.lock();
					resourcesManager.RegisterResource(std::move(mat.value()));
					mutex.unlock();
				}

				mat.reset();
				skip = false;

				ERM_ASSERT(splitted.size() >= 2);
				std::string name = splitted[splitted.size() - 1];
				mutex.lock();
				const auto& materials = resourcesManager.GetResources<Material>();
				const auto& it = std::find_if(materials.begin(), materials.end(), [name](const Material& mat) {
					(void)mat;
//					TODO: Damiano
//					return mat.mAssetID == name;
					return false;
				});
				if (it != materials.end())
				{
					skip = true;
				}
				mutex.unlock();

				mat = Material::DEFAULT;
//				mat->mAssetID = name;
			}
			else if (skip)
			{
				continue;
			}
			else if (splitted[0].compare("Ka") == 0)
			{
				ERM_ASSERT(splitted.size() >= 4);
				mat->mAmbient = math::vec3(
					std::atof(splitted[splitted.size() - 3].c_str()),
					std::atof(splitted[splitted.size() - 2].c_str()),
					std::atof(splitted[splitted.size() - 1].c_str()));
			}
			else if (splitted[0].compare("Kd") == 0)
			{
				ERM_ASSERT(splitted.size() >= 4);
				mat->mDiffuse = math::vec3(
					std::atof(splitted[splitted.size() - 3].c_str()),
					std::atof(splitted[splitted.size() - 2].c_str()),
					std::atof(splitted[splitted.size() - 1].c_str()));
			}
			else if (splitted[0].compare("Ks") == 0)
			{
				ERM_ASSERT(splitted.size() >= 4);
				mat->mSpecular = math::vec3(
					std::atof(splitted[splitted.size() - 3].c_str()),
					std::atof(splitted[splitted.size() - 2].c_str()),
					std::atof(splitted[splitted.size() - 1].c_str()));
			}
			else if (splitted[0].compare("Ns") == 0)
			{
				ERM_ASSERT(splitted.size() >= 2);
				mat->mShininess = static_cast<float>(std::atof(splitted[splitted.size() - 1].c_str()));
			}
			else if (splitted[0].compare("map_Kd") == 0)
			{
				if (splitted.size() != 2)
					continue;
				mutex.lock();
//				TODO: Damiano
				mat->mTexturesMaps[erm::TextureType::DIFFUSE] = StringID(splitted[1]);
				mutex.unlock();
			}
			else if (splitted[0].compare("map_Kn") == 0)
			{
				if (splitted.size() != 2)
					continue;
				mutex.lock();
//				TODO: Damiano
				mat->mTexturesMaps[erm::TextureType::NORMAL] = StringID(splitted[1]);
				mutex.unlock();
			}
		}
	}

	stream.close();

	if (!skip && mat)
	{
		mutex.lock();
		resourcesManager.RegisterResource(std::move(mat.value()));
		mutex.unlock();
	}

	return true;
}

} // namespace erm
