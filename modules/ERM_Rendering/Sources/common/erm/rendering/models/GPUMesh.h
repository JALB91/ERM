#pragma once

#include "erm/rendering/buffers/BufferHandle.h"

namespace erm {
struct Mesh;
}

namespace erm {

class GPUMesh
{
public:
	GPUMesh(const Mesh& mesh)
		: mMesh(mesh)
		, mVertBufferHandle(nullptr, {})
		, mIndBufferHandle(nullptr, {})
	{}

	virtual ~GPUMesh() = default;

	GPUMesh(GPUMesh&& other) noexcept
		: mMesh(other.mMesh)
		, mVertBufferHandle(std::move(other.mVertBufferHandle))
		, mIndBufferHandle(std::move(other.mIndBufferHandle))
	{}
	GPUMesh(const GPUMesh&) = delete;

	GPUMesh& operator=(GPUMesh&& other) = delete;
	GPUMesh& operator=(const GPUMesh&) = delete;
	
	inline const Mesh& getMesh() const { return mMesh; }

	inline const BufferHandle& getVertBufferHandle() const { return mVertBufferHandle; }
	inline void setVertBufferHandle(BufferHandle&& handle) { mVertBufferHandle = std::move(handle); }

	inline const BufferHandle& getIndBufferHandle() const { return mIndBufferHandle; }
	inline void setIndBufferHandle(BufferHandle&& handle) { mIndBufferHandle = std::move(handle); }

protected:
	const Mesh& mMesh;
	BufferHandle mVertBufferHandle;
	BufferHandle mIndBufferHandle;
	
};

} // namespace erm
