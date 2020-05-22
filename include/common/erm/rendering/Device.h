#pragma once

#include <memory>

struct GLFWwindow;

namespace erm {

	class Device
	{
	public:
		Device(GLFWwindow* window);
		~Device();

		void OnRender();

	private:
		class Impl;
		std::unique_ptr<Impl> mImpl;
	};

} // namespace erm
