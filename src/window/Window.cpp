#include "window/Window.h"

#include "utils/Utils.h"

#include "window/IWindowListener.h"

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <string>
#include <iostream>
#include <functional>

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
	
	const char* const kGlslVersion = "#version 330";
	
	const float kImGuiSpaceUp = 0.0f;
	const float kImGuiSpaceDown = 0.2f;
	const float kImGuiSpaceLeft = 0.2f;
	const float kImGuiSpaceRight = 0.2f;
	
}

namespace internal {
	
	void OnFocus(GLFWwindow* window, int focus)
	{
		static_cast<erm::Window*>(glfwGetWindowUserPointer(window))->OnFocus();
	}
	
	void OnMaximized(GLFWwindow* window, int maximised)
	{
		static_cast<erm::Window*>(glfwGetWindowUserPointer(window))->OnMaximised(maximised == GLFW_TRUE);
	}
	
	void OnSizeChanged(GLFWwindow* window, int width, int height)
	{
		static_cast<erm::Window*>(glfwGetWindowUserPointer(window))->OnSizeChanged(width, height);
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
	
}

namespace erm {
	
	Window::Window()
		: IWindow()
		, mWindow(nullptr)
		, mFirstFocus(true)
	{}
	
	Window::~Window()
	{
		mWindowListeners.clear();
		
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		glfwTerminate();
	}
	
	bool Window::Init()
	{
		if (!glfwInit())
		{
			std::cout << "GLFW initialization failed!" << std::endl;
			return false;
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
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		
		mWindow = glfwCreateWindow(mode->width, mode->height, "ERM", nullptr, nullptr);
		
		if (!mWindow)
		{
			std::cout << "GLFW Window creation failed!" << std::endl;
			glfwTerminate();
			return false;
		}
		
		glfwMakeContextCurrent(mWindow);
		glfwSwapInterval(1);
		
		if (glewInit() != GLEW_OK)
		{
			std::cout << "GLEW initialization failed" << std::endl;
			glfwTerminate();
			return false;
		}
		
		glfwSetWindowUserPointer(mWindow, this);
		
		glfwSetWindowFocusCallback(mWindow, internal::OnFocus);
		glfwSetWindowMaximizeCallback(mWindow, internal::OnMaximized);
		glfwSetWindowSizeCallback(mWindow, internal::OnSizeChanged);
		glfwSetMouseButtonCallback(mWindow, internal::OnMouseButton);
		glfwSetCursorPosCallback(mWindow, internal::OnMousePos);
		glfwSetKeyCallback(mWindow, internal::OnKey);
		
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
		ImGui_ImplOpenGL3_Init(kGlslVersion);
		
		return true;
	}
	
	bool Window::ShouldClose()
	{
		return glfwWindowShouldClose(mWindow) || IsKeyDown(KEY_ESCAPE);
	}
	
	void Window::NewFrame()
	{
		mPrevMousePosX = mMousePosX;
		mPrevMousePosY = mMousePosY;
		glfwGetCursorPos(mWindow, &mMousePosX, &mMousePosY);
		
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}
	
	void Window::Render()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		glfwSwapBuffers(mWindow);
	}
	
	void Window::PostRender()
	{
		glfwPollEvents();
	}
	
	void Window::OnKey(int key, int scanCode, int action, int mods)
	{
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
	
	void Window::OnMouseButton(int button, int action, int mods)
	{
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
		mWindowWidth = width;
		mWindowHeight = height;
		UpdateViewport();
		UpdateAspectRatio();
		SafeForEach<IWindowListener>(mWindowListeners, [this] (IWindowListener* listener) {
			listener->OnSizeChanged(mWindowWidth, mWindowHeight);
		});
	}
	
	void Window::OnMaximised(bool wasMaximised)
	{}
	
	void Window::OnFocus()
	{
		if (mFirstFocus)
		{
			mFirstFocus = false;
			int width, height;
			glfwGetWindowSize(mWindow, &width, &height);
			OnSizeChanged(width, height);
		}
	}
	
	void Window::UpdateViewport()
	{
		mViewport.y = std::max(mWindowHeight * 0.1f, mWindowHeight - (mWindowHeight * kImGuiSpaceUp + mWindowHeight * kImGuiSpaceDown));
		mViewport.x = std::max(mWindowWidth * 0.1f, mWindowWidth - (mWindowWidth * kImGuiSpaceRight + mWindowWidth * kImGuiSpaceLeft));
		
		float scaleX, scaleY;
		glfwGetWindowContentScale(mWindow, &scaleX, &scaleY);
		
		GLCALL(glViewport(
			mWindowWidth * kImGuiSpaceLeft * scaleX,
			mWindowHeight * kImGuiSpaceDown * scaleY,
			mViewport.x * scaleX,
			mViewport.y * scaleY
		));
	}

	void Window::UpdateAspectRatio()
	{
		mAspectRatio = mViewport.x / mViewport.y;
	}
	
}
