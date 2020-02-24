#pragma once

#include "ecs/IComponent.h"

#include "math/mat.h"

#include "rendering/data_structs/Bone.h"

#include <array>
#include <memory>

namespace erm {
	namespace ecs {
		
		class SkeletonSystem;
		
		static const BoneId MAX_BONES = 50;
		static const BoneId INVALID_BONE = -1;
		
		struct SkeletonComponent : public IComponent
		{
		public:
			typedef SkeletonSystem SYSTEM_TYPE;
			friend class SkeletonSystem;
				
		public:
			std::array<std::unique_ptr<Bone>, MAX_BONES> mRootBone;
			
		};
		
	}
}
