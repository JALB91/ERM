#include "rendering/Texture.h"

#include "utils/Utils.h"

#include <stb_image.h>

#include <GL/glew.h>

#include <iostream>
#include <fstream>

namespace erm {
	
	bool Texture::Create(
		const char* path,
		std::deque<Texture>& texturesContainer
	)
	{
		std::ifstream stream (Utils::GetRelativePath(path));
		
		if (!stream.is_open())
		{
			std::cout << "No such file: " << path << std::endl;
			return false;
		}

		stream.close();
		
		texturesContainer.emplace_back(path);
		
		return true;
	}
	
	Texture::Texture(const char* path)
		: mRendererId(0)
		, mLocalBuffer(nullptr)
		, mPath(path)
		, mWidth(0)
		, mHeight(0)
		, mBPP(0)
	{
		stbi_set_flip_vertically_on_load(1);
		mLocalBuffer = stbi_load(path, &mWidth, &mHeight, &mBPP, 4);
		
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
