#pragma once

#include "erm/rendering/data_structs/IAsset.h"

namespace erm {
class Device;
}

namespace erm {

class CubeMap : public IAsset
{
public:
	CubeMap(Device& device, const char* path);

	void Init() {};

private:
	Device& mDevice;

};

}