#pragma once

#include "erm/resources/data_structs/IAsset.h"

#include <string_view>

namespace erm {

class Texture : public IAsset
{
public:
	Texture(std::string_view path);
	Texture(std::string_view name, uint32_t width, uint32_t height);
	~Texture();

	Texture(Texture&&) = delete;
	Texture(const Texture&) = delete;

	Texture& operator=(Texture&&) = delete;
	Texture& operator=(const Texture&) = delete;
	
	inline const unsigned char* GetBuffer() const { return mBuffer; }
	inline int GetWidth() const { return mWidth; }
	inline int GetHeight() const { return mHeight; }
	inline uint32_t GetArrayLayers() const { return mArrayLayers; }
	
	inline uint32_t GetMipLevels() const { return mMipLevels; }
	inline void SetMipLevels(uint32_t mipLevels) { mMipLevels = mipLevels; }
	
protected:
	unsigned char* mBuffer;
	uint32_t mWidth, mHeight, mBPP;
	uint32_t mMipLevels, mArrayLayers;

};

} // namespace erm
