#pragma once

#include "ISceneObject.h"

#include <glm/glm.hpp>

#include <memory>

namespace erm {

	class Model;
	class Texture;
	class ShaderProgram;
	
	class SceneObject: public ISceneObject
	{
	public:
		SceneObject(Model model);
		virtual ~SceneObject();
		
		// ISceneObject
		inline void OnUpdate() override {}
		void OnImGuiRender() override;
		void OnRender(const Renderer& renderer) override;
		
		void OnMouseDrag(double deltaX, double deltaY) override;
		
	private:
		std::unique_ptr<Model> mModel;
		std::unique_ptr<Texture> mTexture;
		std::unique_ptr<ShaderProgram> mShader;
		
		int mDragMode;
		
	};
	
}
