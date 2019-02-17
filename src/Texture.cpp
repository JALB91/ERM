#include "Texture.h"
#include "Renderer.h"

#include <stb_image.h>

#include <GLFW/glfw3.h>

namespace erm {
	
	Texture::Texture(const std::string& path)
		: mRendererId(0)
		, mFilePath(path)
		, mLocalBuffer(nullptr)
		, mWidth(0)
		, mHeight(0)
		, mBPP(0)
	{
		stbi_set_flip_vertically_on_load(1);
		mLocalBuffer = stbi_load(path.c_str(), &mWidth, &mHeight, &mBPP, 4);
		
		GLCALL(glGenTextures(1, &mRendererId));
		GLCALL(glBindTexture(GL_TEXTURE_2D, mRendererId));
		
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		
		GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, mLocalBuffer));
		
		GLCALL(glBindTexture(GL_TEXTURE_2D, 0));
		
		if (mLocalBuffer)
		{
			stbi_image_free(mLocalBuffer);
		}
	}
	
	Texture::~Texture()
	{
		GLCALL(glDeleteTextures(1, &mRendererId));
	}
	
	void Texture::Bind(unsigned int slot /* = 0 */) const
	{
		GLCALL(glActiveTexture(GL_TEXTURE0 + slot));
		GLCALL(glBindTexture(GL_TEXTURE_2D, mRendererId));
	}
	
	void Texture::Unbind() const
	{
		GLCALL(glBindTexture(GL_TEXTURE_2D, 0));
	}
	
}
