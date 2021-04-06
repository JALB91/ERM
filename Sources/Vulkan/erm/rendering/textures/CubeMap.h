#pragma once

#include "erm/rendering/textures/Texture.h"

namespace erm {
class Device;
}

namespace erm {

class CubeMap : public Texture
{
public:
	CubeMap(Device& device, const char* path);
	~CubeMap();

	CubeMap(CubeMap&&) = delete;
	CubeMap(const CubeMap&) = delete;

	CubeMap& operator=(CubeMap&&) = delete;
	CubeMap& operator=(const CubeMap&) = delete;

private:
	void CreateTextureImage() override;
	void CreateTextureImageView() override;
};

} // namespace erm
