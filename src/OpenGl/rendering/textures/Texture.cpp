#include "erm/rendering/textures/Texture.h"

#include "erm/GlMacros.h"

#include <stb_image.h>

#include <GL/glew.h>

#include <iostream>
#include <fstream>

namespace erm {
	
	Texture::Texture(const char* path)
		: mRendererId(0)
		, mLocalBuffer(nullptr)
		, mWidth(0)
		, mHeight(0)
		, mBPP(0)
		, mPath(path)
	{
		stbi_set_flip_vertically_on_load(1);
		mLocalBuffer = stbi_load(path, &mWidth, &mHeight, &mBPP, 4);
		
		GL_CALL(glGenTextures(1, &mRendererId));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, mRendererId));
		
		GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		
		GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, mLocalBuffer));
		
		GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
		
		if (mLocalBuffer)
		{
			stbi_image_free(mLocalBuffer);
		}
	}
	
	Texture::~Texture()
	{
		GL_CALL(glDeleteTextures(1, &mRendererId));
	}
	
	void Texture::Bind(unsigned int slot /* = 0 */) const
	{
		GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, mRendererId));
	}
	
	void Texture::Unbind() const
	{
		GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
	}
	
}
