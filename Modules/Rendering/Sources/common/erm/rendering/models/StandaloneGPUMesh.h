#pragma once

#include "erm/rendering/buffers/IndexBuffer.h"
#include "erm/rendering/buffers/VertexBuffer.h"
#include "erm/rendering/models/GPUMesh.h"

namespace erm {
class Device;
}

namespace erm {

class StandaloneGPUMesh : public GPUMesh
{
public:
	StandaloneGPUMesh(Device& device, const Mesh& mesh);
	~StandaloneGPUMesh();

	StandaloneGPUMesh(StandaloneGPUMesh&& other) noexcept;
	StandaloneGPUMesh(const StandaloneGPUMesh&) = delete;

	StandaloneGPUMesh& operator=(StandaloneGPUMesh&&) = delete;
	StandaloneGPUMesh& operator=(const StandaloneGPUMesh&) = delete;
	
	inline const IndexBuffer& GetIndexBuffer() const { return mIndexBuffer; }
	inline const VertexBuffer& GetVertexBuffer() const { return mVertexBuffer; }

private:
	Device& mDevice;
	IndexBuffer mIndexBuffer;
	VertexBuffer mVertexBuffer;
	
};

} // namespace erm
