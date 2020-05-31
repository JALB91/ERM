#include "erm/rendering/buffers/Buffer.h"

#include "erm/rendering/Device.h"

namespace erm {

	Buffer::Buffer(
		Device& device,
		size_t size)
		: mDevice(device)
		, mBufferSize(size)
	{}

	Buffer::Buffer(Buffer&& other)
		: mDevice(other.mDevice)
		, mBufferSize(other.mBufferSize)
		, mBuffer(std::move(other.mBuffer))
		, mBufferMemory(std::move(other.mBufferMemory))
	{
		other.mBuffer = nullptr;
		other.mBufferMemory = nullptr;
	}

	Buffer::~Buffer()
	{
		if (mBufferMemory && mBuffer)
		{
			mDevice->freeMemory(mBufferMemory);
			mDevice->destroyBuffer(mBuffer);
		}
	}

} // namespace erm
