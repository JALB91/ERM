#pragma once

#include "erm/math/BoundingBox.h"

#include "erm/rendering/data_structs/IAsset.h"
#include "erm/rendering/data_structs/Mesh.h"

#include <string>
#include <vector>

namespace erm {
	class Device;
}

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

		inline bool IsDirty() const { return mIsDirty; }
		inline void SetDirty(bool isDirty) { mIsDirty = isDirty; }

		void AddMesh(Mesh&& mesh);
		void AddMesh(
			std::vector<VertexData>&& vertices,
			std::vector<IndexData>&& indices,
			const RenderConfigs& configs = RenderConfigs::MODELS_RENDER_CONFIGS,
			const char* name = "");
		void UpdateLocalBound();

	private:
		Device& mDevice;
		std::vector<Mesh> mMeshes;
		BoundingBox3D mLocalBounds;
		bool mIsDirty;
	};

} // namespace erm
