#pragma once

#include "erm/math/BoundingBox.h"

#include "erm/rendering/data_structs/Mesh.h"

#include <string>
#include <vector>

namespace erm {
	class Device;
}

namespace erm {

	class Model
	{
	public:
		Model(Device& device, const char* path, const char* name);
		~Model();

		Model(Model&&) = delete;
		Model(const Model&) = delete;

		Model& operator=(Model&&) = delete;
		Model& operator=(const Model&) = delete;

		inline const std::string& GetPath() const { return mPath; }

		inline const std::string& GetName() const { return mName; }
		inline void SetName(const std::string& name) { mName = name; }

		inline const std::vector<Mesh>& GetMeshes() const { return mMeshes; }
		inline std::vector<Mesh>& GetMeshes() { return mMeshes; }

		inline BoundingBox3D GetLocalBounds() { return mLocalBounds; }
		inline const BoundingBox3D& GetLocalBounds() const { return mLocalBounds; }

		inline bool IsDirty() const { return mIsDirty; }
		inline void SetDirty(bool isDirty) { mIsDirty = isDirty; }

		void AddMesh(Mesh&& mesh);
		void AddMesh(
			DrawMode drawMode,
			VertexData* vertices,
			uint32_t verticesCount,
			IndexData* indices,
			uint32_t indicesCount,
			const char* name = "",
			ShaderProgram* shaderProgram = nullptr,
			Material* material = nullptr,
			Texture* texture = nullptr);
		void UpdateLocalBound();

	private:
		Device& mDevice;
		const std::string mPath;
		std::string mName;
		std::vector<Mesh> mMeshes;
		BoundingBox3D mLocalBounds;
		bool mIsDirty;
	};

} // namespace erm
