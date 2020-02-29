#pragma once

#include "ecs/IComponent.h"

#include "rendering/data_structs/Bone.h"

namespace erm {
	namespace ecs {
		
		class SkeletonSystem;
		
		struct SkeletonComponent : public IComponent
		{
			typedef SkeletonSystem SYSTEM_TYPE;
			
			SkeletonComponent(BonesTree* tree = nullptr)
				: mRootBone(tree)
			{}
				
			BonesTree* mRootBone;
		};
		
	}
}
