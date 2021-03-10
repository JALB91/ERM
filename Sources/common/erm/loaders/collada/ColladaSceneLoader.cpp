#include "erm/loaders/collada/ColladaSceneLoader.h"

#include "erm/loaders/collada/ColladaLoaderUtils.h"

#include "erm/rendering/data_structs/Bone.h"
#include "erm/rendering/data_structs/Skin.h"

#include "erm/utils/Utils.h"

#include <tinyxml2.h>

using namespace tinyxml2;

namespace erm {

	void ProcessNode(
		XMLElement& node,
		std::unique_ptr<BonesTree>& root,
		BonesTree* tree,
		math::mat4 parentBind,
		const std::map<std::string, ColladaSkinData>& skinsData);

	void ProcessScene(
		std::mutex& mutex,
		const char* path,
		tinyxml2::XMLDocument& document,
		Skins& skins,
		const std::map<std::string, ColladaSkinData>& skinsData)
	{
		XMLElement* libraryVisualScene = document.RootElement()->FirstChildElement("library_visual_scenes");
		if (!libraryVisualScene)
			return;
		XMLElement* visualScene = libraryVisualScene->FirstChildElement("visual_scene");

		std::unique_ptr<BonesTree> tree = nullptr;

		while (visualScene)
		{
			XMLElement* node = visualScene->FirstChildElement("node");

			while (node)
			{
				ProcessNode(*node, tree, tree.get(), glm::identity<math::mat4>(), skinsData);
				node = node->NextSiblingElement("node");
			}

			visualScene = visualScene->NextSiblingElement("visual_scene");
		}

		if (tree)
		{
			mutex.lock();
			skins.emplace_back(std::make_unique<Skin>(path, path, std::move(tree)));
			mutex.unlock();
		}
	}

	void ProcessNode(
		XMLElement& node,
		std::unique_ptr<BonesTree>& root,
		BonesTree* tree,
		math::mat4 parentBind,
		const std::map<std::string, ColladaSkinData>& skinsData)
	{
		bool found = false;

		if (const char* boneName = node.Attribute("sid"))
		{
			for (const auto& entry : skinsData)
			{
				const ColladaSkinData& skinData = entry.second;

				for (unsigned int i = 0; i < skinData.mBoneNames.size(); ++i)
				{
					if (skinData.mBoneNames[i] == boneName)
					{
						std::vector<std::string> values = Utils::SplitString(node.FirstChildElement("matrix")->GetText(), ' ');
						math::mat4 bindMatrix;

						ParseMatrix(values, 0, bindMatrix);

						parentBind *= bindMatrix;
						const math::mat4 parentInverseBind = glm::inverse(parentBind);

						if (!root)
						{
							root = std::make_unique<BonesTree>(i, std::make_unique<Bone>(bindMatrix, parentInverseBind, boneName));
							tree = root.get();
						}
						else
						{
							tree = &tree->AddChild(i, std::make_unique<Bone>(bindMatrix, parentInverseBind, boneName));
						}

						found = true;
						break;
					}
				}
			}
		}

		if (!found && std::strcmp(node.Attribute("id"), "Armature") == 0)
		{
			math::mat4 bindMatrix = glm::identity<math::mat4>();

			if (XMLElement* matrixElement = node.FirstChildElement("matrix"))
			{
				std::vector<std::string> values = Utils::SplitString(node.FirstChildElement("matrix")->GetText(), ' ');

				ParseMatrix(values, 0, bindMatrix);
			}

			parentBind *= bindMatrix;
		}

		XMLElement* childNode = node.FirstChildElement("node");

		while (childNode)
		{
			ProcessNode(*childNode, root, tree, parentBind, skinsData);
			childNode = childNode->NextSiblingElement("node");
		}
	}

} // namespace erm
