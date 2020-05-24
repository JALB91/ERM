#pragma once

#include "erm/rendering/IDevice.h"

struct GLFWwindow;

namespace erm {

	class Device : public IDevice
	{
	public:
		Device(GLFWwindow* window);

		// IDevice
		void OnRender() override;
	};

} // namespace erm
