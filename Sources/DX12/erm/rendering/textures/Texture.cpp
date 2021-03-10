#include "erm/rendering/textures/Texture.h"

namespace erm {

	Texture::Texture(Device& device, const char* path)
		: IAsset(path, "")
		, mDevice(device)
	{}

} // namespace erm