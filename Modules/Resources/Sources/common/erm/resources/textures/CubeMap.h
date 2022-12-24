#pragma once

#include "erm/resources/data_structs/IAsset.h"

#include <array>
#include <string_view>

namespace erm {

class CubeMap : public IAsset
{
public:
	static constexpr std::array kFaces {
		"right",
		"left",
		"top",
		"bottom",
		"front",
		"back"};
	
public:
	CubeMap(std::string_view path);
	~CubeMap();

	CubeMap(CubeMap&&) = delete;
	CubeMap(const CubeMap&) = delete;

	CubeMap& operator=(CubeMap&&) = delete;
	CubeMap& operator=(const CubeMap&) = delete;

	inline const std::array<unsigned char*, 6>& GetBuffers() const { return mBuffers; }
	inline int GetWidth() const { return mWidth; }
	inline int GetHeight() const { return mHeight; }
	inline uint32_t GetArrayLayers() const { return mArrayLayers; }
	
	inline uint32_t GetMipLevels() const { return mMipLevels; }
	inline void SetMipLevels(uint32_t mipLevels) { mMipLevels = mipLevels; }
	
protected:
	std::array<unsigned char*, kFaces.size()> mBuffers;
	uint32_t mWidth, mHeight, mBPP;
	uint32_t mMipLevels, mArrayLayers;

};

} // namespace erm
