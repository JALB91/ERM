#pragma once

#include "erm/rendering/models/GPUMesh.h"

// clang-format off
#ifdef ERM_RAY_TRACING_ENABLED
#include "erm/ray_tracing/RTBlas.h"
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
	
	inline const Model& GetModel() const { return mModel; }

	inline const std::vector<GPUMesh>& GetGPUMeshes() const { return mGPUMeshes; }
	inline std::vector<GPUMesh>& GetGPUMeshes() { return mGPUMeshes; }

	inline const VertexBuffer& GetVerticesBuffer() const { return *mVerticesBuffer; }
	inline const IndexBuffer& GetIndicesBuffer() const { return *mIndicesBuffer; }

	inline bool IsDirty() const { return mIsDirty; }
	inline void SetDirty(bool isDirty) { mIsDirty = isDirty; }

#ifdef ERM_RAY_TRACING_ENABLED
	inline RTBlas& GetBlas()
	{
		return mBlas;
	}
#endif

	void UpdateBuffers();

private:
	Device& mDevice;
	const Model& mModel;
	std::vector<GPUMesh> mGPUMeshes;
#ifdef ERM_RAY_TRACING_ENABLED
	RTBlas mBlas;
#endif
	std::unique_ptr<VertexBuffer> mVerticesBuffer;
	std::unique_ptr<IndexBuffer> mIndicesBuffer;
	bool mIsDirty;
};

} // namespace erm
