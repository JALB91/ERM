#pragma once

#include "ec/IComponent.h"

#include <memory>

namespace erm {
	
	class Texture;
	class ShaderProgram;
	class ModelComponent;
	class TransformComponent;
	
	class SpriteComponent: public IComponent
	{
	public:
		SpriteComponent(Entity& entity, const char* filePath);
		~SpriteComponent();
		
	private:
		TransformComponent& mTransformComponent;
		ModelComponent& mModelComponent;
		std::unique_ptr<Texture> mTexture;
		std::unique_ptr<ShaderProgram> mShader;
		
	};
	
}
