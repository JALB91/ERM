#include "erm/rendering/buffers/IBuffer.h"

#include "erm/rendering/Device.h"

#include "erm/utils/Utils.h"
#include "erm/utils/VkUtils.h"

namespace erm {

IBuffer::IBuffer(
	Device& device,
	size_t size,
	vk::BufferUsageFlags buf,
	vk::MemoryPropertyFlags mpf)
	: mDevice(device)
	, mBufferSize(size)
	, mBuf(buf)
	, mMpf(mpf)
{
	VkUtils::CreateBufferUnique(
		device.GetVkPhysicalDevice(),
		device.GetVkDevice(),
		size,
		buf,
		mpf,
		mBuffer,
		mBufferMemory);

	mLayout.mBuffer = mBuffer.get();
	mLayout.mInfos.emplace_back(BufferInfo {0, mBufferSize});
}

IBuffer::IBuffer(const IBuffer& other)
	: mDevice(other.mDevice)
	, mBufferSize(other.mBufferSize)
	, mBuf(other.mBuf)
	, mMpf(other.mMpf)
{
	VkUtils::CreateBufferUnique(
		mDevice.GetVkPhysicalDevice(),
		mDevice.GetVkDevice(),
		mBufferSize,
		mBuf,
		mMpf,
		mBuffer,
		mBufferMemory);

	mLayout.mBuffer = mBuffer.get();
	mLayout.mInfos.emplace_back(BufferInfo {0, mBufferSize});
}

IBuffer::IBuffer(IBuffer&& other)
	: mDevice(other.mDevice)
	, mBufferSize(other.mBufferSize)
	, mLayout(std::move(other.mLayout))
	, mBuf(std::move(other.mBuf))
	, mMpf(std::move(other.mMpf))
	, mBuffer(std::move(other.mBuffer))
	, mBufferMemory(std::move(other.mBufferMemory))
{}

IBuffer& IBuffer::operator=(IBuffer&& other)
{
	if (this == &other)
		return *this;

	ASSERT(&mDevice == &other.mDevice);
	mBufferSize = other.mBufferSize;
	mBuf = std::move(other.mBuf);
	mMpf = std::move(other.mMpf);
	mBuffer = std::move(other.mBuffer);
	mBufferMemory = std::move(other.mBufferMemory);

	return *this;
}

} // namespace erm
