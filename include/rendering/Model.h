#pragma once

#include "math/BoundingBox.h"

#include "rendering/Mesh.h"

#include <deque>
#include <string>

namespace erm {
	
	class Model
	{
	public:
		Model(const char* path, const char* name);
		~Model();
		
		Model(Model&&) = delete;
		Model(const Model&) = delete;
		
		Model& operator=(Model&&) = delete;
		Model& operator=(const Model&) = delete;
		
		inline const std::string& GetPath() const { return mPath; }

		inline const std::string& GetName() const { return mName; }
		inline void SetName(const std::string& name) { mName = name; }
		
		inline const std::deque<Mesh>& GetMeshes() const { return mMeshes; }
		inline std::deque<Mesh>& GetMeshes() { return mMeshes; }
		
		inline BoundingBox3D GetLocalBounds() { return mLocalBounds; }
		inline const BoundingBox3D& GetLocalBounds() const { return mLocalBounds; }

		void AddMesh(Mesh&& mesh);
		void UpdateLocalBound();
		
	private:
		const std::string mPath;
		std::string mName;
		std::deque<Mesh> mMeshes;
		BoundingBox3D mLocalBounds;
		
	};
	
}
