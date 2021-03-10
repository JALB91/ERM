#pragma once

struct GLFWwindow;

namespace erm {

	class Device
	{
	public:
		Device(GLFWwindow* window);

	private:
		GLFWwindow* mWindow;
	};

} // namespace erm