#include "erm/rendering/Device.h"

#include "erm/utils/Utils.h"

namespace erm {
	
	class Device::Impl
	{};
	
	Device::Device(GLFWwindow* window)
	{
		UNUSED(window);
		UNUSED(mImpl);
	}
	
	Device::~Device() = default;
	
	void Device::OnRender()
	{}
	
}
