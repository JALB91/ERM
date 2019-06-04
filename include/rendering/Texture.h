#pragma once

#include <string>

namespace erm {
	
	class Texture
	{
	public:
		Texture(const char* filePath);
		~Texture();
		
		Texture(Texture&&) = delete;
		Texture(const Texture&) = delete;
		
		Texture& operator=(Texture&&) = delete;
		Texture& operator=(const Texture&) = delete;
		
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
