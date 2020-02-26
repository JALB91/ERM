#pragma once

#include "ecs/IComponent.h"

#include "math/mat.h"

#include "rendering/data_structs/Bone.h"

#include "utils/Tree.h"

#include <array>
#include <memory>

namespace erm {
	namespace ecs {
		
		class SkeletonSystem;
		
		typedef Tree<BoneId, std::unique_ptr<Bone>> BonesTree;
		
		struct SkeletonComponent : public IComponent
		{
			typedef SkeletonSystem SYSTEM_TYPE;
			
			SkeletonComponent(BonesTree tree)
				: mRootBone(std::move(tree))
			{}
				
			BonesTree mRootBone;
		};
		
	}
}
