#pragma once

#include <string>

namespace erm {
	
	class Texture
	{
	public:
		Texture(const char* filePath);
		~Texture();
		
		void Bind(unsigned int slot = 0) const;
		void Unbind() const;
		
		inline int GetWidth() const { return mWidth; }
		inline int GetHeight() const { return mHeight; }
		
		inline const std::string& GetFilePath() const { return mFilePath; }
		
	private:
		unsigned int mRendererId;
		unsigned char* mLocalBuffer;
		std::string mFilePath;
		int mWidth, mHeight, mBPP;
		
	};
	
}
