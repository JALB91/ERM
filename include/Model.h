#pragma once

#include <glm/glm.hpp>

#include <string>
#include <vector>

namespace erm {
	
	class Mesh;
	
	class Model
	{
	public:
		Model(const std::string& path);
		~Model();
		
		inline glm::vec3& GetTranslation() { return mTranslation; }
		inline const glm::vec3& GetTranslation() const { return mTranslation; }
		inline void SetTranslation(const glm::vec3& translation) { mTranslation = translation; }
		
		inline glm::vec3& GetRotation() { return mRotation; }
		inline const glm::vec3& GetRotation() const { return mRotation; }
		inline void SetRotation(const glm::vec3& rotation) { mRotation = rotation; }
		
		inline const std::vector<Mesh>& GetMeshes() const { return mMeshes; }
		
	private:
		glm::vec3 mTranslation;
		glm::vec3 mRotation;
		
		std::vector<Mesh> mMeshes;
		
	};
	
}
