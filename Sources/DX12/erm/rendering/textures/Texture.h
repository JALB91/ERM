#pragma once

#include "erm/rendering/data_structs/IAsset.h"

namespace erm {
	class Device;
}

namespace erm {

	class Texture : public IAsset
	{
	public:
		Texture(Device& device, const char* path);

	private:
		Device& mDevice;
	};

} // namespace erm