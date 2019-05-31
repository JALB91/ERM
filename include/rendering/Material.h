#pragma once

#include <string>
#include <memory>

namespace erm {
	
	class ShaderProgram;
	class Texture;
	
	class Material
	{
	public:
		Material(
			const char* vertexShaderPath,
			const char* fragmentShaderPath,
			const char* texturePath
		);
		Material(
			const char* shaderPath,
			const char* texturePath
		);
		
		inline const ShaderProgram& GetShader() const { return *mShader; }
		inline const Texture& GetTexture() const { return *mTexture; }
		
	private:
		std::unique_ptr<ShaderProgram> mShader;
		std::unique_ptr<Texture> mTexture;
		
	};
	
}
