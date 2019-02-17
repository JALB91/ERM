#pragma once

#include <string>

namespace erm {
	
	class Texture
	{
	public:
		Texture(const std::string& path);
		~Texture();
		
		void Bind(unsigned int slot = 0) const;
		void Unbind() const;
		
		inline int GetWidth() const { return mWidth; }
		inline int GetHeight() const { return mHeight; }
		
	private:
		unsigned int mRendererId;
		std::string mFilePath;
		unsigned char* mLocalBuffer;
		int mWidth, mHeight, mBPP;
		
	};
	
}
