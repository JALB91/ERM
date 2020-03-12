#pragma once

#include "erm/ecs/IComponent.h"

#include "erm/rendering/data_structs/Bone.h"

namespace erm {
	namespace ecs {
		
		class SkeletonSystem;
		
		struct SkeletonComponent : public IComponent
		{
			typedef SkeletonSystem SYSTEM_TYPE;
			friend class SkeletonSystem;
			
			SkeletonComponent(BonesTree* tree = nullptr)
				: mRootBone(tree)
			{}
			
			SENSIBLE_MEMBER(RootBone, BonesTree*, mRootBone);
			
		private:
			BonesTree* mRootBone;
			
		};
		
	}
}