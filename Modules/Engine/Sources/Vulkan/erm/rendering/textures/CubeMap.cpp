#include "erm/rendering/textures/CubeMap.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/buffers/HostBuffer.h"

#include "erm/utils/Utils.h"
#include "erm/utils/VkUtils.h"

#include <stb_image.h>

#include <array>
#include <filesystem>

namespace {

const std::array kFaces {
	"right",
	"left",
	"top",
	"bottom",
	"front",
	"back"};

}

namespace erm {

CubeMap::CubeMap(Device& device)
	: Texture(device)
{
	mImageViewType = vk::ImageViewType::eCube;
}

CubeMap::~CubeMap() = default;

void CubeMap::CreateTextureImage()
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

	std::unique_ptr<HostBuffer> stagingBuffer;

	for (size_t i = 0; i < kFaces.size(); ++i)
	{
		int width, height, bPP;
		mLocalBuffer = stbi_load((mPath + "/" + kFaces[i] + targetExt).c_str(), &width, &height, &bPP, STBI_rgb_alpha);

		mWidth = static_cast<uint32_t>(width);
		mHeight = static_cast<uint32_t>(height);
		mBPP = static_cast<uint32_t>(bPP);
		vk::DeviceSize imageSize = mWidth * mHeight * 4;

		ERM_ASSERT(mLocalBuffer);

		if (!stagingBuffer)
			stagingBuffer = std::make_unique<HostBuffer>(mDevice, imageSize * kFaces.size(), BufferUsage::TRANSFER_SRC);
		stagingBuffer->Update(mLocalBuffer, BufferInfo(i * imageSize, imageSize));

		stbi_image_free(mLocalBuffer);
	}

	mFormat = vk::Format::eR8G8B8A8Srgb;
	mMipLevels = 1;
	mArrayLayers = static_cast<uint32_t>(kFaces.size());

	vk::ImageCreateInfo imageInfo {};
	imageInfo.imageType = vk::ImageType::e2D;
	imageInfo.extent.width = GetWidth();
	imageInfo.extent.height = GetHeight();
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = GetMipLevels();
	imageInfo.arrayLayers = GetArrayLayers();
	imageInfo.format = GetImageFormat();
	imageInfo.tiling = vk::ImageTiling::eOptimal;
	imageInfo.initialLayout = GetImageLayout();
	imageInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
	imageInfo.samples = vk::SampleCountFlagBits::e1;
	imageInfo.sharingMode = vk::SharingMode::eExclusive;
	imageInfo.flags = vk::ImageCreateFlagBits::eCubeCompatible;

	VkUtils::CreateImage(
		mDevice.GetVkPhysicalDevice(),
		mDevice.GetVkDevice(),
		imageInfo,
		mImage,
		mImageMemory,
		MemoryProperty::DEVICE_LOCAL);

	TransitionImageLayout(vk::ImageLayout::eTransferDstOptimal);

	VkUtils::CopyBufferToImage(
		mDevice,
		stagingBuffer->GetBuffer(),
		GetImage(),
		static_cast<uint32_t>(GetWidth()),
		static_cast<uint32_t>(GetHeight()),
		GetArrayLayers());
}

} // namespace erm