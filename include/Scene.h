#pragma once

#include "ISceneObject.h"

#include <glm/glm.hpp>

#include <memory>

namespace erm {
	
	class VertexBuffer;
	class VertexBufferLayout;
	class VertexArray;
	class IndexBuffer;
	class ShaderProgram;
	
	class Scene: ISceneObject
	{
	public:
		Scene(
			const float* vertices,
			unsigned int verticesCount,
			const unsigned int* indices,
			unsigned int indicesCount
		);
		~Scene();
		
		// ISceneObject
		inline void OnUpdate() override {};
		void OnImGuiRender() override;
		void OnRender(const Renderer& renderer) override;
		
	private:
		glm::vec3 mTranslation;
		glm::vec3 mRotation;
		
		std::unique_ptr<VertexBuffer> mVB;
		std::unique_ptr<VertexBufferLayout> mVBL;
		std::unique_ptr<IndexBuffer> mIB;
		std::unique_ptr<VertexArray> mVA;
		std::unique_ptr<ShaderProgram> mShader;
		
	};
	
}
