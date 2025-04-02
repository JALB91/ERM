#pragma once

#include "erm/rendering/models/GPUMesh.h"

// clang-format off
#ifdef ERM_RAY_TRACING_ENABLED
#include "erm/rendering/ray_tracing/RTBlas.h"
#endif
// clang-format on

#include <memory>
#include <vector>

namespace erm {
class Device;
class Model;
class IndexBuffer;
class VertexBuffer;
} // namespace erm

namespace erm {

class GPUModel
{
public:
	GPUModel(Device& device, const Model& model);
	~GPUModel();

	GPUModel(GPUModel&&) = delete;
	GPUModel(const GPUModel&) = delete;

	GPUModel& operator=(GPUModel&&) = delete;
	GPUModel& operator=(const GPUModel&) = delete;
	
	inline const Model& getModel() const { return mModel; }

	inline const std::vector<GPUMesh>& getGPUMeshes() const { return mGPUMeshes; }

	inline const VertexBuffer& getVerticesBuffer() const { return *mVerticesBuffer; }
	inline const IndexBuffer& getIndicesBuffer() const { return *mIndicesBuffer; }

#ifdef ERM_RAY_TRACING_ENABLED
	inline RTBlas& getBlas()
	{
		return mBlas;
	}
#endif

	void updateBuffers();

private:
	Device& mDevice;
	const Model& mModel;
	std::vector<GPUMesh> mGPUMeshes;
#ifdef ERM_RAY_TRACING_ENABLED
	RTBlas mBlas;
#endif
	std::unique_ptr<VertexBuffer> mVerticesBuffer;
	std::unique_ptr<IndexBuffer> mIndicesBuffer;
	
};

} // namespace erm
