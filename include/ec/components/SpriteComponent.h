#pragma once

#include "ec/IComponent.h"

#include <memory>

namespace erm {
	
	class Texture;
	class ShaderProgram;
	class MeshComponent;
	class TransformComponent;
	
	class SpriteComponent: public IComponent
	{
	public:
		SpriteComponent(Entity& entity, const char* filePath);
		~SpriteComponent();
		
	private:
		TransformComponent& mTransformComponent;
		MeshComponent& mMeshComponent;
		std::unique_ptr<Texture> mTexture;
		std::unique_ptr<ShaderProgram> mShader;
		
	};
	
}
