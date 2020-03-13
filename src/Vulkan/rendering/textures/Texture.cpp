#include "erm/rendering/textures/Texture.h"

#include "erm/utils/Utils.h"

#include <stb_image.h>

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
		UNUSED(mRendererId);
		
		stbi_set_flip_vertically_on_load(1);
		mLocalBuffer = stbi_load(path, &mWidth, &mHeight, &mBPP, 4);
		
		if (mLocalBuffer)
		{
			stbi_image_free(mLocalBuffer);
		}
	}
	
	Texture::~Texture()
	{}
	
	void Texture::Bind(unsigned int /*slot*/ /* = 0 */) const
	{}
	
	void Texture::Unbind() const
	{}
	
}
