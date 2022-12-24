#pragma once

#include "erm/resources/data_structs/Bone.h"
#include "erm/resources/data_structs/IAsset.h"

#include <memory>

namespace erm {

struct Skin : public IAsset
{
	Skin(
		std::string_view path,
		std::string_view name,
		std::unique_ptr<BonesTree> bonesTree)
		: IAsset(path, name)
		, mRootBone(std::move(bonesTree))
	{}

	std::unique_ptr<BonesTree> mRootBone;
};

} // namespace erm
