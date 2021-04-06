#pragma once

#include "erm/math/BoundingBox.h"

#include "erm/rendering/data_structs/IAsset.h"
#include "erm/rendering/data_structs/Mesh.h"

// clang-format off
#ifdef ERM_RAY_TRACING_ENABLED
#include "erm/ray_tracing/RTBlas.h"
#endif
// clang-format on

#include <memory>
#include <vector>

namespace erm {
class Device;
class IndexBuffer;
class VertexBuffer;
} // namespace erm

namespace erm {

class Model : public IAsset
{
public:
	Model(Device& device, const char* path, const char* name);
	~Model();

	Model(Model&&) = delete;
	Model(const Model&) = delete;

	Model& operator=(Model&&) = delete;
	Model& operator=(const Model&) = delete;

	inline const std::vector<Mesh>& GetMeshes() const { return mMeshes; }
	inline std::vector<Mesh>& GetMeshes() { return mMeshes; }

	inline BoundingBox3D GetLocalBounds() { return mLocalBounds; }
	inline const BoundingBox3D& GetLocalBounds() const { return mLocalBounds; }

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

	void AddMesh(Mesh&& mesh);
	void AddMesh(
		std::vector<VertexData>&& vertices,
		std::vector<IndexData>&& indices,
		const PipelineConfigs& configs = PipelineConfigs::DEFAULT_PIPELINE_CONFIGS,
		const char* name = "");
	void UpdateLocalBound();
	void UpdateBuffers();

private:
	Device& mDevice;
	std::vector<Mesh> mMeshes;
	BoundingBox3D mLocalBounds;
#ifdef ERM_RAY_TRACING_ENABLED
	RTBlas mBlas;
#endif
	std::unique_ptr<VertexBuffer> mVerticesBuffer;
	std::unique_ptr<IndexBuffer> mIndicesBuffer;
	bool mIsDirty;
};

} // namespace erm
