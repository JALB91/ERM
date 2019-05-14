#include "ec/components/SpriteComponent.h"
#include "ec/components/TransformComponent.h"
#include "ec/components/ModelComponent.h"
#include "ec/Entity.h"

#include "rendering/Mesh.h"
#include "rendering/Texture.h"
#include "rendering/ShaderProgram.h"
#include "rendering/Uniforms.h"

#include "utils/MeshUtils.h"

namespace erm {
	
	SpriteComponent::SpriteComponent(Entity& entity, const char* filePath)
		: IComponent(entity)
		, mTransformComponent(entity.RequireComponent<TransformComponent>())
		, mModelComponent(entity.RequireComponent<ModelComponent>(MeshUtils::CreateSquare()))
		, mTexture(std::make_unique<Texture>(filePath))
		, mShader(std::make_unique<ShaderProgram>("res/shaders/texture"))
	{
		mShader->Bind();
		mTexture->Bind();
		mShader->SetUniform1i(Uniform::TEXTURE_2D, 0);
	}
	
	SpriteComponent::~SpriteComponent()
	{}
	
}
