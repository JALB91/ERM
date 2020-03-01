#include "loaders/collada/ColladaAnimationsLoader.h"

#include "loaders/collada/ColladaLoaderUtils.h"

#include "math/math.h"

#include "utils/Utils.h"

#include <tinyxml2.h>

using namespace tinyxml2;

namespace erm {
	
	void ProcessAnimations(
		std::mutex& mutex,
		tinyxml2::XMLDocument& document,
		const std::vector<std::string>& boneNames,
		const char* path,
		Animations& animations
	)
	{
		XMLElement* animationsLibrary = document.RootElement()->FirstChildElement("library_animations");
		if (!animationsLibrary) return;
		XMLElement* animationSource = animationsLibrary->FirstChildElement("animation");
		
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
										ParseMatrix(values, i, matrices.emplace_back());
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
					Pose& currentPose = keyFrames[i].mTransforms[targetBone];
					math::DecomposeMatrix(
						matrices[i],
						currentPose.mTranslation,
						currentPose.mRotation,
						currentPose.mScale
					);
				}
			}
			animationSource = animationSource->NextSiblingElement("animation");
		}
		
		mutex.lock();
		animations.emplace_back(std::make_unique<SkeletonAnimation>(keyFrames, keyFrames.back().mTimestamp, path));
		mutex.unlock();
	}
	
}
