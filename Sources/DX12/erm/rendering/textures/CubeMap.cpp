#include "erm/rendering/textures/CubeMap.h"

namespace erm {

CubeMap::CubeMap(Device& device, const char* path)
	: IAsset(path, "")
	, mDevice(device)
{}

}