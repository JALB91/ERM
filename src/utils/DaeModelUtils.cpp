#include "utils/DaeModelUtils.h"
#include "utils/Utils.h"

#include "rendering/buffers/VertexData.h"
#include "rendering/buffers/IndexData.h"

#include <tinyxml2.h>

#include <iostream>

using namespace tinyxml2;

namespace erm {
	
	void ReadVerticesSources(
		XMLElement* mesh,
		std::vector<PositionVertex>& pVertices,
		std::vector<NormalVertex>& nVertices,
		std::vector<UVVertex>& uvVertices
	);
	
	void ReadBonesSources(
		XMLElement* skin,
		std::vector<BoneIds>& boneIds,
		std::vector<BoneWeights>& boneWeigths,
		std::vector<std::string>& boneNames,
		std::vector<math::mat4>& inverseBinds
	);
	
	void ProcessNode(
		XMLElement* node,
		const std::vector<std::string> boneNames,
		std::vector<math::mat4>& inverseBinds,
		BonesTree* tree,
		math::mat4 parentBind,
		math::mat4 parentInverseBind
	);
	
	void ReadMatrix(
		std::vector<std::string>& values,
		unsigned int offset,
		math::mat4& matrix
	);
	
	void ParseDaeModel(
		std::mutex& mutex,
		std::atomic<bool>& stop,
		const char* path,
		Model& model,
		Materials& /*materials*/,
		Skins& skins,
		Animations& animations
	)
	{
		XMLDocument document;
		XMLError error = document.LoadFile(path);
		
		if (error != XMLError::XML_SUCCESS)
		{
			std::cout << "Error %d while reading dae file %s" << error << path << std::endl;
			return;
		}
		
		std::vector<BoneIds> boneIds;
		std::vector<BoneWeights> boneWeights;
		std::vector<std::string> boneNames;
		std::vector<math::mat4> inverseBinds;
		
		if (XMLElement* controllers = document.RootElement()->FirstChildElement("library_controllers"))
		{
			XMLElement* controller = controllers->FirstChildElement("controller");
			
			while (controller)
			{
				if (XMLElement* skin = controller->FirstChildElement("skin"))
				{
					std::string skinSource = skin->Attribute("source");
					skinSource = skinSource.substr(1);
					
					ReadBonesSources(skin, boneIds, boneWeights, boneNames, inverseBinds);
				}
				
				controller = controller->NextSiblingElement("controller");
			}
		}

		XMLElement* geometry =
		  document.RootElement()->FirstChildElement("library_geometries")->FirstChildElement("geometry");

		while (geometry)
		{
			if (stop) return;
			
			const std::string name = geometry->Attribute("id");
			
			XMLElement* mesh = geometry->FirstChildElement("mesh");
			
			while (mesh)
			{
				XMLElement* primitive = mesh->FirstChildElement("polylist");
				bool isValid = true;
				
				if (!primitive)
				{
					primitive = mesh->FirstChildElement("triangles");
					
					if (!primitive)
					{
						mesh = mesh->NextSiblingElement("mesh");
						continue;
					}
				}
				else
				{
					for (const std::string& value : Utils::SplitString(primitive->FirstChildElement("vcount")->GetText(), ' '))
					{
						isValid &= (std::atoi(value.c_str()) == 3);
					}
				}
				
				if (!isValid)
				{
					mesh = mesh->NextSiblingElement("mesh");
					continue;
				}
				
				std::vector<PositionVertex> pVertices;
				std::vector<NormalVertex> nVertices;
				std::vector<UVVertex> uvVertices;
				
				ReadVerticesSources(mesh, pVertices, nVertices, uvVertices);
				
				int primCount;
				primitive->QueryIntAttribute("count", &primCount);
				const int indicesCount = primCount * 3;
				
				int totalOffset = 0;
				int pOffset = -1;
				int nOffset = -1;
				int uvOffset = -1;
				
				XMLElement* input = primitive->FirstChildElement("input");
				
				while (input)
				{
					int offset;
					input->QueryIntAttribute("offset", &offset);
					
					if (std::strcmp(input->Attribute("semantic"), "VERTEX") == 0)
					{
						pOffset = offset;
					}
					else if (std::strcmp(input->Attribute("semantic"), "NORMAL") == 0)
					{
						nOffset = offset;
					}
					else if (std::strcmp(input->Attribute("semantic"), "TEXCOORD") == 0)
					{
						uvOffset = offset;
					}
					
					totalOffset = std::max(totalOffset, offset);
					
					input = input->NextSiblingElement("input");
				}
				
				++totalOffset;
				
				const std::vector<std::string> values = Utils::SplitString(primitive->FirstChildElement("p")->GetText(), ' ');
				
				IndexData* indicesData = new IndexData[indicesCount];
				VertexData* verticesData = new VertexData[indicesCount];
				
				for (unsigned int i = 0; i < static_cast<unsigned int>(values.size()); i += totalOffset)
				{
					const unsigned int index = i / totalOffset;
					VertexData vertexData;
					
					indicesData[index] = index;
					
					const unsigned short pIndex = std::atoi(values[i + pOffset].c_str());
					vertexData.mPositionVertex = pVertices[pIndex];
					vertexData.mBoneIds = boneIds[pIndex];
					vertexData.mBoneWeights = boneWeights[pIndex];
					
					if (!nVertices.empty() && nOffset >= 0)
					{
						const unsigned short nIndex = std::atoi(values[i+nOffset].c_str());
						vertexData.mNormalVertex = nVertices[nIndex];
					}
					
					if (!uvVertices.empty() && uvOffset >= 0)
					{
						const unsigned short uvIndex = std::atoi(values[i+uvOffset].c_str());
						vertexData.mUVVertex = uvVertices[uvIndex];
					}
					
					verticesData[index] = vertexData;
				}
				
				mutex.lock();
				model.AddMesh(Mesh(DrawMode::TRIANGLES, verticesData, indicesCount, indicesData, indicesCount, nullptr, name));
				mutex.unlock();
				
				mesh = mesh->NextSiblingElement("mesh");
			}
			geometry = geometry->NextSiblingElement("geometry");
		}
		
		XMLElement* visualScene = document.RootElement()->FirstChildElement("library_visual_scenes")->FirstChildElement("visual_scene");
		std::unique_ptr<BonesTree> tree (std::make_unique<BonesTree>(0, nullptr));
		
		while (visualScene)
		{
			XMLElement* node = visualScene->FirstChildElement("node");
			
			while (node)
			{
				ProcessNode(node, boneNames, inverseBinds, tree.get(), glm::identity<math::mat4>(), glm::identity<math::mat4>());
				node = node->NextSiblingElement("node");
			}
			
			visualScene = visualScene->NextSiblingElement("visual_scene");
		}
		
		mutex.lock();
		skins.emplace_back(std::move(tree));
		mutex.unlock();
		
		XMLElement* animationSource = document.RootElement()->FirstChildElement("library_animations")->FirstChildElement("animation");
		
		std::vector<KeyFrame> keyFrames;
		std::vector<float> timestamps;
		
		while (animationSource)
		{
			XMLElement* samplerElement = animationSource->FirstChildElement("sampler");
			std::string boneId = animationSource->FirstChildElement("channel")->Attribute("target");
			boneId = boneId.substr(0, boneId.find("/"));
			
			int targetBone = -1;
			
			for (unsigned int i = 0; i < boneNames.size(); ++i)
			{
				if (boneNames[i] == boneId)
				{
					targetBone = i;
					break;
				}
			}
			
			if (targetBone < 0)
			{
				continue;
			}
			
			if (samplerElement)
			{
				std::vector<math::mat4> matrices;
				
				XMLElement* input = samplerElement->FirstChildElement("input");
				
				while (input)
				{
					bool isInput = std::strcmp(input->Attribute("semantic"), "INPUT") == 0;
					bool isOutput = std::strcmp(input->Attribute("semantic"), "OUTPUT") == 0;
					
					if (isInput || isOutput)
					{
						std::string targetSource = input->Attribute("source");
						targetSource = targetSource.substr(1);
						
						XMLElement* source = animationSource->FirstChildElement("source");
						
						while (source)
						{
							if (source->Attribute("id") == targetSource)
							{
								std::vector<std::string> values = Utils::SplitString(source->FirstChildElement("float_array")->GetText(), ' ');
								
								if (isInput && values.size() > timestamps.size())
								{
									for (unsigned int i = timestamps.size(); i < values.size(); ++i)
									{
										timestamps.emplace_back(std::atof(values[i].c_str()));
									}
								}
								else if (isOutput)
								{
									for (int i = 0; i < source->FirstChildElement("technique_common")->FirstChildElement("accessor")->IntAttribute("count"); ++i)
									{
										ReadMatrix(values, i, matrices.emplace_back());
									}
								}
							}
							source = source->NextSiblingElement("source");
						}
					}
					input = input->NextSiblingElement("input");
				}
				
				for (unsigned int i = keyFrames.size(); i < timestamps.size(); ++i)
				{
					keyFrames.emplace_back(KeyFrame(timestamps[i]));
				}
				
				for (unsigned int i = 0; i < keyFrames.size(); ++i)
				{
					math::vec3 translation = math::vec3(matrices[i][3][0], matrices[i][3][1], matrices[i][3][2]);
					math::quat quat = glm::quat_cast(matrices[i]);
					keyFrames[i].mTransforms[targetBone].mTranslation = translation;
					keyFrames[i].mTransforms[targetBone].mRotation = quat;
					keyFrames[i].mTransforms[targetBone].mScale = math::vec3(1.0f);
				}
			}
			animationSource = animationSource->NextSiblingElement("animation");
		}
		
		std::unique_ptr<SkeletonAnimation> skeletonAnimation (std::make_unique<SkeletonAnimation>(keyFrames, keyFrames.back().mTimestamp, path));
		
		mutex.lock();
		animations.emplace_back(std::move(skeletonAnimation));
		mutex.unlock();
	}
	
	void ProcessNode(
		XMLElement* node,
		const std::vector<std::string> boneNames,
		std::vector<math::mat4>& inverseBinds,
		BonesTree* tree,
		math::mat4 parentBind,
		math::mat4 parentInverseBind
	)
	{
		bool found = false;
		
		for (unsigned int i = 0; i < static_cast<unsigned int>(boneNames.size()); ++i)
		{
			const std::string& boneName = boneNames[i];
			if (node->Attribute("sid") && boneName == node->Attribute("sid"))
			{
				std::vector<std::string> values = Utils::SplitString(node->FirstChildElement("matrix")->GetText(), ' ');
				math::mat4 bindMatrix;
				
				ReadMatrix(values, 0, bindMatrix);
				
				parentBind *= bindMatrix;
				parentInverseBind = inverseBinds[i];
				
				tree = &tree->AddChild(i, std::make_unique<Bone>(parentInverseBind, boneName.c_str()));
				found = true;
				break;
			}
		}
		
		if (!found && std::strcmp(node->Attribute("id"), "Armature") == 0)
		{
			math::mat4 bindMatrix = glm::identity<math::mat4>();
			
			if (XMLElement* matrixElement = node->FirstChildElement("matrix"))
			{
				std::vector<std::string> values = Utils::SplitString(node->FirstChildElement("matrix")->GetText(), ' ');

				ReadMatrix(values, 0, bindMatrix);
			}

			parentBind *= bindMatrix;
			parentInverseBind = glm::inverse(bindMatrix);
		}
		
		XMLElement* childNode = node->FirstChildElement("node");
		
		while (childNode)
		{
			ProcessNode(childNode, boneNames, inverseBinds, tree, parentBind, parentInverseBind);
			childNode = childNode->NextSiblingElement("node");
		}
	}
	
	void ReadBonesSources(
		XMLElement* skin,
		std::vector<BoneIds>& boneIds,
		std::vector<BoneWeights>& boneWeigths,
		std::vector<std::string>& boneNames,
		std::vector<math::mat4>& inverseBinds
	)
	{
		XMLElement* joints = skin->FirstChildElement("joints");
		XMLElement* jointInput = joints->FirstChildElement("input")->NextSiblingElement("input");
		std::string inverseBindsSourceStr = jointInput->Attribute("source");
		inverseBindsSourceStr = inverseBindsSourceStr.substr(1);
		
		XMLElement* vertexWeights = skin->FirstChildElement("vertex_weights");
		XMLElement* input = vertexWeights->FirstChildElement("input");
		std::string jointsSourceStr = input->Attribute("source");
		std::string weightsSourceStr = input->NextSiblingElement("input")->Attribute("source");
		
		jointsSourceStr = jointsSourceStr.substr(1);
		weightsSourceStr = weightsSourceStr.substr(1);
		
		std::vector<float> weights;
		
		XMLElement* sourceElement = skin->FirstChildElement("source");
		
		while (sourceElement)
		{
			if (sourceElement->Attribute("id") == jointsSourceStr)
			{
				boneNames = Utils::SplitString(sourceElement->FirstChildElement("Name_array")->GetText(), ' ');
			}
			else if (sourceElement->Attribute("id") == weightsSourceStr)
			{
				for (std::string& value : Utils::SplitString(sourceElement->FirstChildElement("float_array")->GetText(), ' '))
				{
					weights.emplace_back(std::atof(value.c_str()));
				}
			}
			else if (sourceElement->Attribute("id") == inverseBindsSourceStr)
			{
				std::vector<std::string> values = Utils::SplitString(sourceElement->FirstChildElement("float_array")->GetText(), ' ');
				for (unsigned int i = 0; i < static_cast<unsigned int>(values.size()); i += 16)
				{
					ReadMatrix(values, i/16, inverseBinds.emplace_back());
				}
			}
			
			sourceElement = sourceElement->NextSiblingElement("source");
		}
		
		std::vector<unsigned int> counts;
		
		for (std::string& count : Utils::SplitString(vertexWeights->FirstChildElement("vcount")->GetText(), ' '))
		{
			counts.emplace_back(std::atoi(count.c_str()));
		}
		
		std::vector<unsigned int> values;
		
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

	void ReadVerticesSources(
		XMLElement* mesh,
		std::vector<PositionVertex>& pVertices,
		std::vector<NormalVertex>& nVertices,
		std::vector<UVVertex>& uvVertices
	)
	{
		std::string pSourceStr = mesh->FirstChildElement("vertices")->FirstChildElement("input")->Attribute("source");
		pSourceStr = pSourceStr.erase(0, 1);
		XMLElement* pSource = mesh->FirstChildElement("source");
		while (pSource)
		{
			if (pSource->Attribute("id") == pSourceStr) break;
			pSource = pSource->NextSiblingElement("source");
		}
		std::string pSourceVecStr = pSource->FirstChildElement("float_array")->GetText();
		std::vector<std::string> pSourceVec (Utils::SplitString(pSourceVecStr, ' '));
		
		ASSERT(pSourceVec.size() % 3 == 0);
		
		for (unsigned int i = 0; i < static_cast<unsigned int>(pSourceVec.size()); i += 3)
		{
			pVertices.emplace_back(
				atof(pSourceVec[i].c_str()),
				atof(pSourceVec[i+1].c_str()),
				atof(pSourceVec[i+2].c_str())
			);
		}
		
		XMLElement* firstInput = mesh->FirstChildElement("polylist");
		
		if (!firstInput)
		{
			firstInput = mesh->FirstChildElement("triangles")->FirstChildElement("input")->NextSiblingElement("input");
		}
		else
		{
			firstInput = firstInput->FirstChildElement("input")->NextSiblingElement("input");
		}
		
		const std::string firstInputSemantic = firstInput->Attribute("semantic");
		
		if (!firstInput) return;
		
		XMLElement* secondInput = firstInput->NextSiblingElement("input");
		
		XMLElement* uvInput = nullptr;
		XMLElement* nInput = nullptr;
		
		
		if (firstInputSemantic == "TEXCOORD") uvInput = firstInput;
		else if (firstInputSemantic == "NORMAL") nInput = firstInput;
		
		if (secondInput)
		{
			const std::string secondInputSemantic = secondInput->Attribute("semantic");
			if (secondInputSemantic == "TEXCOORD") uvInput = secondInput;
			else if (secondInputSemantic == "NORMAL") nInput = secondInput;
		}
		
		if (nInput)
		{
			std::string nSourceStr = nInput->Attribute("source");
			nSourceStr = nSourceStr.erase(0, 1);
			XMLElement* nSource = mesh->FirstChildElement("source");
			while (nSource)
			{
				if (nSource->Attribute("id") == nSourceStr) break;
				nSource = nSource->NextSiblingElement("source");
			}
			if (nSource)
			{
				std::string nSourceVecStr = nSource->FirstChildElement("float_array")->GetText();
				std::vector<std::string> nSourceVec (Utils::SplitString(nSourceVecStr, ' '));
				
				ASSERT(nSourceVec.size() % 3 == 0);
				
				for (unsigned int i = 0; i < static_cast<unsigned int>(nSourceVec.size()); i += 3)
				{
					nVertices.emplace_back(
						atof(nSourceVec[i].c_str()),
						atof(nSourceVec[i+1].c_str()),
						atof(nSourceVec[i+2].c_str())
					);
				}
			}
		}
		
		
		if (uvInput)
		{
			std::string uvSourceStr = uvInput->Attribute("source");
			uvSourceStr = uvSourceStr.erase(0, 1);
			XMLElement* uvSource = mesh->FirstChildElement("source");
			while (uvSource)
			{
				if (uvSource->Attribute("id") == uvSourceStr) break;
				uvSource = uvSource->NextSiblingElement("source");
			}
			if (uvSource)
			{
				std::string uvSourceVecStr = uvSource->FirstChildElement("float_array")->GetText();
				std::vector<std::string> uvSourceVec (Utils::SplitString(uvSourceVecStr, ' '));
				
				ASSERT(uvSourceVec.size() % 2 == 0);
				
				for (unsigned int i = 0; i < static_cast<unsigned int>(uvSourceVec.size()); i += 2)
				{
					uvVertices.emplace_back(
						atof(uvSourceVec[i].c_str()),
						atof(uvSourceVec[i+1].c_str())
					);
				}
			}
		}
	}
	void ReadMatrix(
		std::vector<std::string>& values,
		unsigned int offset,
		math::mat4& matrix
	)
	{
		matrix[0][0] = std::atof(values[offset*16+0].c_str());
		matrix[1][0] = std::atof(values[offset*16+1].c_str());
		matrix[2][0] = std::atof(values[offset*16+2].c_str());
		matrix[3][0] = std::atof(values[offset*16+3].c_str());

		matrix[0][1] = std::atof(values[offset*16+4].c_str());
		matrix[1][1] = std::atof(values[offset*16+5].c_str());
		matrix[2][1] = std::atof(values[offset*16+6].c_str());
		matrix[3][1] = std::atof(values[offset*16+7].c_str());

		matrix[0][2] = std::atof(values[offset*16+8].c_str());
		matrix[1][2] = std::atof(values[offset*16+9].c_str());
		matrix[2][2] = std::atof(values[offset*16+10].c_str());
		matrix[3][2] = std::atof(values[offset*16+11].c_str());

		matrix[0][3] = std::atof(values[offset*16+12].c_str());
		matrix[1][3] = std::atof(values[offset*16+13].c_str());
		matrix[2][3] = std::atof(values[offset*16+14].c_str());
		matrix[3][3] = std::atof(values[offset*16+15].c_str());
	}
}
