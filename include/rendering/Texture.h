#pragma once

#include <string>
#include <vector>

namespace erm {
	
	class Texture
	{
	public:
		Texture(const char* path);
		~Texture();
		
		Texture(Texture&&) = delete;
		Texture(const Texture&) = delete;
		
		Texture& operator=(Texture&&) = delete;
		Texture& operator=(const Texture&) = delete;
		
		void Bind(unsigned int slot = 0) const;
		void Unbind() const;
		
		inline int GetWidth() const { return mWidth; }
		inline int GetHeight() const { return mHeight; }
		
		inline const std::string& GetPath() const { return mPath; }
		
	private:
		unsigned int mRendererId;
		unsigned char* mLocalBuffer;
		int mWidth, mHeight, mBPP;
		const std::string mPath;
		
	};
	
}
