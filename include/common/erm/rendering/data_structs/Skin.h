#pragma once

#include "erm/rendering/data_structs/Bone.h"
#include "erm/rendering/data_structs/IAsset.h"

#include <memory>

namespace erm {

	struct Skin : public IAsset
	{
		Skin(
			const char* path,
			const char* name,
			std::unique_ptr<BonesTree> bonesTree)
			: IAsset(path, name)
			, mRootBone(std::move(bonesTree))
		{}

		std::unique_ptr<BonesTree> mRootBone;
	};

} // namespace erm