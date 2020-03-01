#include "loaders/collada/ColladaSceneLoader.h"

#include "loaders/collada/ColladaLoaderUtils.h"

#include "utils/Utils.h"

#include <tinyxml2.h>

using namespace tinyxml2;

namespace erm {
	
	void ProcessNode(
		XMLElement& node,
		BonesTree* tree,
		math::mat4 parentBind,
		math::mat4 parentInverseBind,
		const std::vector<std::string>& boneNames
	);
	
	void ProcessScene(
		std::mutex& mutex,
		XMLDocument& document,
		const std::vector<std::string>& boneNames,
		Skins& skins
	)
	{
		XMLElement* libraryVisualScene = document.RootElement()->FirstChildElement("library_visual_scenes");
		if (!libraryVisualScene) return;
		XMLElement* visualScene = libraryVisualScene->FirstChildElement("visual_scene");
		
		std::unique_ptr<BonesTree> tree = std::make_unique<BonesTree>(0, Bone());
		
		while (visualScene)
		{
			XMLElement* node = visualScene->FirstChildElement("node");
			
			while (node)
			{
				ProcessNode(*node, tree.get(), glm::identity<math::mat4>(), glm::identity<math::mat4>(), boneNames);
				node = node->NextSiblingElement("node");
			}
			
			visualScene = visualScene->NextSiblingElement("visual_scene");
		}
		
		if (tree->GetSize() > 1)
		{
			mutex.lock();
			skins.emplace_back(std::move(tree));
			mutex.unlock();
		}
	}
	
	void ProcessNode(
		XMLElement& node,
		BonesTree* tree,
		math::mat4 parentBind,
		math::mat4 parentInverseBind,
		const std::vector<std::string>& boneNames
	)
	{
		bool found = false;
		
		for (unsigned int i = 0; i < static_cast<unsigned int>(boneNames.size()); ++i)
		{
			const std::string& boneName = boneNames[i];
			if (node.Attribute("sid") && boneName == node.Attribute("sid"))
			{
				std::vector<std::string> values = Utils::SplitString(node.FirstChildElement("matrix")->GetText(), ' ');
				math::mat4 bindMatrix;
				
				ParseMatrix(values, 0, bindMatrix);
				
				parentBind *= bindMatrix;
				parentInverseBind = glm::inverse(parentBind);
				
				tree = &tree->AddChild(i, Bone(bindMatrix, parentInverseBind, boneName.c_str()));
				found = true;
				break;
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
			parentInverseBind = glm::inverse(bindMatrix);
		}
		
		XMLElement* childNode = node.FirstChildElement("node");
		
		while (childNode)
		{
			ProcessNode(*childNode, tree, parentBind, parentInverseBind, boneNames);
			childNode = childNode->NextSiblingElement("node");
		}
	}
	
}
