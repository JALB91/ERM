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
	
	inline const Mesh& GetMesh() const { return mMesh; }

	inline const BufferHandle& GetVertBufferHandle() const { return mVertBufferHandle; }
	inline void SetVertBufferHandle(BufferHandle&& handle) { mVertBufferHandle = std::move(handle); }

	inline const BufferHandle& GetIndBufferHandle() const { return mIndBufferHandle; }
	inline void SetIndBufferHandle(BufferHandle&& handle) { mIndBufferHandle = std::move(handle); }

protected:
	const Mesh& mMesh;
	BufferHandle mVertBufferHandle;
	BufferHandle mIndBufferHandle;
	
};

} // namespace erm
