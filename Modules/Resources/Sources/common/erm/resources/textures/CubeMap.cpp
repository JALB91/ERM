#include "erm/resources/textures/CubeMap.h"

#include <erm/utils/Utils.h>

#include <stb_image.h>

#include <cmath>
#include <filesystem>

namespace erm {

CubeMap::CubeMap(std::string_view path)
	: IAsset(path, path)
	, mBuffers{nullptr}
	, mWidth(0)
	, mHeight(0)
	, mBPP(0)
	, mMipLevels(1)
	, mArrayLayers(kFaces.size())
{
	stbi_set_flip_vertically_on_load(0);

	std::string targetExt;

	for (const auto& entry : std::filesystem::directory_iterator(mPath))
	{
		if (entry.path().has_extension())
		{
			targetExt = entry.path().extension().string();
			break;
		}
	}

	for (size_t i = 0; i < kFaces.size(); ++i)
	{
		int width, height, bPP;
		mBuffers[i] = stbi_load((mPath + "/" + kFaces[i] + targetExt).c_str(), &width, &height, &bPP, STBI_rgb_alpha);
		
		ERM_ASSERT(mBuffers[i]);

		mWidth = static_cast<uint32_t>(width);
		mHeight = static_cast<uint32_t>(height);
		mBPP = static_cast<uint32_t>(bPP);
	}
}

CubeMap::~CubeMap()
{
	for (auto*& buffer : mBuffers)
	{
		if (buffer != nullptr)
		{
			stbi_image_free(buffer);
			buffer = nullptr;
		}
	}
}

} // namespace erm
