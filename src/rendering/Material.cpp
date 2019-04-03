#include "rendering/Material.h"
#include "rendering/ShaderProgram.h"
#include "rendering/Texture.h"

namespace erm {
	
	Material::Material(
		const char* vertexShaderPath,
		const char* fragmentShaderPath,
		const char* texturePath
		)
		: mShader(std::make_unique<ShaderProgram>(vertexShaderPath, fragmentShaderPath))
		, mTexture(std::make_unique<Texture>(texturePath))
	{}
	
	Material::Material(
		const char* shaderPath,
		const char* texturePath
		)
		: mShader(std::make_unique<ShaderProgram>(shaderPath))
		, mTexture(std::make_unique<Texture>(texturePath))
	{}
	
}
