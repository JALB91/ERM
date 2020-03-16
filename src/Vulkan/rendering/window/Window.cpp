#include "erm/rendering/window/Window.h"
#include "erm/rendering/window/IWindowListener.h"
#include "erm/rendering/shaders/ShaderProgram.h"

#include "erm/utils/Utils.h"
#include "erm/utils/VkUtils.h"
#include "erm/utils/Profiler.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>

#include <array>
#include <string>
#include <iostream>
#include <algorithm>
#include <functional>
#include <optional>
#include <fstream>

/*
	WINDOW STUFF
*/
namespace {
	
	template <typename T>
	void SafeForEach(const std::set<T*>& set, const std::function<void(T* ptr)>& func)
	{
		std::set<T*> copy = set;
		for (T* t: copy)
		{
			if (t) func(t);
		}
	}
	
	const float kImGuiSpaceUp = 0.0f;
	const float kImGuiSpaceDown = 0.3f;
	const float kImGuiSpaceLeft = 0.2f;
	const float kImGuiSpaceRight = 0.2f;

	bool firstRefresh = true;
	
}

/*
	WINDOW CALLBACKS
*/
namespace internal {
	
	void OnFocus(GLFWwindow* window, int /*focus*/)
	{
		static_cast<erm::Window*>(glfwGetWindowUserPointer(window))->OnFocus();
	}
	
	void OnMouseButton(GLFWwindow* window, int button, int action, int mods)
	{
		static_cast<erm::Window*>(glfwGetWindowUserPointer(window))->OnMouseButton(button, action, mods);
	}
	
	void OnMousePos(GLFWwindow* window, double xPos, double yPos)
	{
		static_cast<erm::Window*>(glfwGetWindowUserPointer(window))->OnMousePos(xPos, yPos);
	}
	
	void OnKey(GLFWwindow* window, int key, int scanCode, int action, int mods)
	{
		static_cast<erm::Window*>(glfwGetWindowUserPointer(window))->OnKey(key, scanCode, action, mods);
	}
	
	void OnFrameBufferResize(GLFWwindow* window, int width, int height)
	{
		static_cast<erm::Window*>(glfwGetWindowUserPointer(window))->OnSizeChanged(width, height);
	}
	
	void OnRefresh(GLFWwindow* window)
	{
		if (firstRefresh)
		{
			firstRefresh = false;
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);
			static_cast<erm::Window*>(glfwGetWindowUserPointer(window))->OnSizeChanged(width, height);
		}
	}
	
}

/*
	WINDOW IMPL
*/
namespace erm {
	
	Window::Window()
		: IWindow()
	{}
	
	Window::~Window()
	{
		mWindowListeners.clear();
		
		glfwTerminate();
	}
	
	bool Window::Init()
	{
		if (!glfwInit())
		{
			throw std::runtime_error("GLFW initialization failed!");
		}
		
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		
		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
		
		glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		
		mWindow = glfwCreateWindow(mode->width, mode->height, "ERM Vulkan", nullptr, nullptr);
		
		if (!mWindow)
		{
			throw std::runtime_error("GLFW Window creation failed!");
		}
		
		glfwMakeContextCurrent(mWindow);
		glfwSwapInterval(1);
		glfwSetWindowUserPointer(mWindow, this);

		glfwSetWindowFocusCallback(mWindow, internal::OnFocus);
		glfwSetMouseButtonCallback(mWindow, internal::OnMouseButton);
		glfwSetCursorPosCallback(mWindow, internal::OnMousePos);
		glfwSetKeyCallback(mWindow, internal::OnKey);
		glfwSetWindowRefreshCallback(mWindow, internal::OnRefresh);
		glfwSetFramebufferSizeCallback(mWindow, internal::OnFrameBufferResize);
		
		return true;
	}
	
	bool Window::ShouldClose()
	{
		return glfwWindowShouldClose(mWindow) || IsKeyDown(KEY_ESCAPE);
	}
	
	void Window::NewFrame()
	{
		PROFILE_FUNCTION();
		
		mPrevMousePosX = mMousePosX;
		mPrevMousePosY = mMousePosY;
		glfwGetCursorPos(mWindow, &mMousePosX, &mMousePosY);
	}
	
	void Window::OnRender()
	{
		PROFILE_FUNCTION();
		
		glfwSwapBuffers(mWindow);
	}
	
	void Window::OnPostRender()
	{
		PROFILE_FUNCTION();
		
		glfwPollEvents();
	}
	
	void Window::OnKey(int key, int /*scanCode*/, int action, int /*mods*/)
	{
		PROFILE_FUNCTION();
		
		if (action != GLFW_RELEASE)
		{
			return;
		}
		
		if (action == GLFW_PRESS)
		{
			mPressedKeys.insert(key);
			SafeForEach<IWindowListener>(mWindowListeners, [key] (IWindowListener* listener) {
				listener->OnKeyPressed(key);
			});
		}
		else if (action == GLFW_RELEASE)
		{
			auto it = mPressedKeys.find(key);
			if (it != mPressedKeys.end())
			{
				mPressedKeys.erase(it);
				SafeForEach<IWindowListener>(mWindowListeners, [key] (IWindowListener* listener) {
					listener->OnKeyReleased(key);
				});
			}
		}
	}
	
	void Window::OnMouseButton(int button, int action, int /*mods*/)
	{
		PROFILE_FUNCTION();
		
		if (action != GLFW_RELEASE)
		{
			return;
		}
		
		if (action == GLFW_PRESS)
		{
			mPressedButtons.insert(button);
			SafeForEach<IWindowListener>(mWindowListeners, [button] (IWindowListener* listener) {
				listener->OnMouseButtonPressed(button);
			});
		}
		else if (action == GLFW_RELEASE)
		{
			auto it = mPressedButtons.find(button);
			if (it != mPressedButtons.end())
			{
				mPressedButtons.erase(it);
				SafeForEach<IWindowListener>(mWindowListeners, [button] (IWindowListener* listener) {
					listener->OnMouseButtonReleased(button);
				});
			}
		}
	}
	
	void Window::OnMousePos(double xPos, double yPos)
	{
		SafeForEach<IWindowListener>(mWindowListeners, [xPos, yPos] (IWindowListener* listener) {
			listener->OnMouseMoved(xPos, yPos);
		});
	}
	
	void Window::OnSizeChanged(int width, int height)
	{
		PROFILE_FUNCTION();
		
		mWindowWidth = width;
		mWindowHeight = height;
		UpdateViewport();
		UpdateAspectRatio();
		SafeForEach<IWindowListener>(mWindowListeners, [this] (IWindowListener* listener) {
			listener->OnSizeChanged(mWindowWidth, mWindowHeight);
		});
	}
	
	void Window::OnMaximised(bool /*wasMaximised*/)
	{
		int width, height;
		glfwGetWindowSize(mWindow, &width, &height);
		OnSizeChanged(width, height);
	}
	
	void Window::OnFocus()
	{
		int width, height;
		glfwGetWindowSize(mWindow, &width, &height);
		
		if (mWindowWidth == width && mWindowHeight == height) return;

		OnSizeChanged(width, height);
	}
	
	void Window::UpdateViewport()
	{
		int width, height;
		glfwGetFramebufferSize(mWindow, &width, &height);

		mViewport.x = std::max(mWindowWidth * 0.1f, mWindowWidth - (mWindowWidth * kImGuiSpaceRight + mWindowWidth * kImGuiSpaceLeft));
		mViewport.y = std::max(mWindowHeight * 0.1f, mWindowHeight - (mWindowHeight * kImGuiSpaceUp + mWindowHeight * kImGuiSpaceDown));
	}

	void Window::UpdateAspectRatio()
	{
		if (mViewport.y > 0.0f)
		{
			mAspectRatio = mViewport.x / mViewport.y;
		}
		else
		{
			mAspectRatio = 0.0f;
		}
	}
	
}
