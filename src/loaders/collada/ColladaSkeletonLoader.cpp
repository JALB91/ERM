#include "loaders/collada/ColladaSkeletonLoader.h"

#include "loaders/collada/ColladaLoaderUtils.h"

#include "utils/Utils.h"

#include <tinyxml2.h>

using namespace tinyxml2;

namespace erm {
	
	void GatherSources(
		XMLElement& skin,
		XMLElement& vertexWeights,
		std::vector<float>& weights,
		std::vector<std::string>& boneNames
	);
	
	void ProcessSkeleton(
		XMLDocument& document,
		std::vector<BoneIds>& boneIds,
		std::vector<BoneWeights>& boneWeigths,
		std::vector<std::string>& boneNames
	)
	{
		XMLElement* controllers = document.RootElement()->FirstChildElement("library_controllers");
		if (!controllers) return;
		
		XMLElement* controller = controllers->FirstChildElement("controller");
		XMLElement* skin = controller->FirstChildElement("skin");
		
		XMLElement* vertexWeights = skin->FirstChildElement("vertex_weights");
		const unsigned int count = vertexWeights->IntAttribute("count");
		
		boneIds.reserve(count);
		boneWeigths.reserve(count);
		
		std::vector<float> weights;
		std::vector<unsigned int> counts;
		std::vector<unsigned int> values;
		
		GatherSources(*skin, *vertexWeights, weights, boneNames);
		
		for (std::string& count : Utils::SplitString(vertexWeights->FirstChildElement("vcount")->GetText(), ' '))
		{
			counts.emplace_back(std::atoi(count.c_str()));
		}
		
		for (std::string& value : Utils::SplitString(vertexWeights->FirstChildElement("v")->GetText(), ' '))
		{
			values.emplace_back(std::atoi(value.c_str()));
		}
		
		unsigned int currentIndex = 0;
		
		for (unsigned int i = 0; i < static_cast<unsigned int>(counts.size()); ++i)
		{
			boneIds.emplace_back(BoneIds(0));
			boneWeigths.emplace_back(0.0f);
			for (unsigned int j = 0; j < counts[i]; ++j)
			{
				if (j > kMaxBonesNumber - 1) break;
				
				boneIds[i][j] = values[currentIndex + j * 2];
				boneWeigths[i][j] = weights[values[currentIndex + j * 2 + 1]];
			}
			currentIndex += counts[i] * 2;
		}
	}
	
	void GatherSources(
		XMLElement& skin,
		XMLElement& vertexWeights,
		std::vector<float>& weights,
		std::vector<std::string>& boneNames
	)
	{
		XMLElement* jointsInput = FindChildWithAttribute(vertexWeights, "input", "semantic", "JOINT");
		XMLElement* weightsInput = FindChildWithAttribute(vertexWeights, "input", "semantic", "WEIGHT");
		
		std::string jointsSourceStr = jointsInput->Attribute("source");
		std::string weightsSourceStr = weightsInput->Attribute("source");
		
		jointsSourceStr = jointsSourceStr.substr(1);
		weightsSourceStr = weightsSourceStr.substr(1);
		
		XMLElement* jointsSource = FindChildWithAttribute(skin, "source", "id", jointsSourceStr.c_str());
		XMLElement* weigthsSource = FindChildWithAttribute(skin, "source", "id", weightsSourceStr.c_str());
		
		boneNames = Utils::SplitString(jointsSource->FirstChildElement("Name_array")->GetText(), ' ');
		for (std::string& value : Utils::SplitString(weigthsSource->FirstChildElement("float_array")->GetText(), ' '))
		{
			weights.emplace_back(std::atof(value.c_str()));
		}
	}
	
}
