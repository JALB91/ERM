#pragma once

#include "ISceneObject.h"

#include <glm/glm.hpp>

#include <memory>

namespace erm {

	class Mesh;
	class Texture;
	class ShaderProgram;
	
	class SceneObject: public ISceneObject
	{
	public:
		SceneObject();
		virtual ~SceneObject();
		
		// ISceneObject
		inline void OnUpdate() override {}
		void OnImGuiRender() override;
		void OnRender(const Renderer& renderer) override;
		
	private:
		std::unique_ptr<Mesh> mMesh;
		std::unique_ptr<Texture> mTexture;
		std::unique_ptr<ShaderProgram> mShader;
		
	};
	
}
