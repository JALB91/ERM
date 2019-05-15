#include "window/Window.h"

#include "utils/Utils.h"

#include "interfaces/IKeyListener.h"
#include "interfaces/IMouseListener.h"
#include "interfaces/IWindowSizeListener.h"

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
	
	const char* const kGlslVersion = "#version 150";
	
}

namespace internal {
	
	void OnFocus(GLFWwindow* window, int focus)
	{
		static_cast<erm::Window*>(glfwGetWindowUserPointer(window))->OnSizeChanged();
	}
	
	void OnMaximized(GLFWwindow* window, int maximised)
	{
		static_cast<erm::Window*>(glfwGetWindowUserPointer(window))->OnSizeChanged();
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
	
	Window::Window(int width, int height)
		: mWindow(nullptr)
		, mMousePosX(0.0)
		, mPrevMousePosX(0.0)
		, mMousePosY(0.0)
		, mPrevMousePosY(0.0)
		, mWindowWidth(width)
		, mWindowHeight(height)
	{
		UpdateAspectRatio();
	}
	
	Window::~Window()
	{
		mKeyListeners.clear();
		mMouseListeners.clear();
		mWindowSizeListeners.clear();
		
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
		
		glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		
#if defined(__APPLE__)
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
		
		mWindow = glfwCreateWindow(mWindowWidth, mWindowHeight, "ERM", nullptr, nullptr);
		
		if (!mWindow)
		{
			std::cout << "GLFW Window creation failed!" << std::endl;
			glfwTerminate();
			return false;
		}
		
		glfwMakeContextCurrent(mWindow);
		glfwSwapInterval(1);
		
		UpdateViewport();
		
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
		return glfwWindowShouldClose(mWindow);
	}
	
	void Window::NewFrame()
	{
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
			SafeForEach<IKeyListener>(mKeyListeners, [key] (IKeyListener* listener) {
				listener->OnKeyPressed(key);
			});
		}
		else if (action == GLFW_RELEASE)
		{
			auto it = mPressedKeys.find(key);
			if (it != mPressedKeys.end())
			{
				mPressedKeys.erase(it);
				SafeForEach<IKeyListener>(mKeyListeners, [key] (IKeyListener* listener) {
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
			SafeForEach<IMouseListener>(mMouseListeners, [button] (IMouseListener* listener) {
				listener->OnMouseButtonPressed(button);
			});
		}
		else if (action == GLFW_RELEASE)
		{
			auto it = mPressedButtons.find(button);
			if (it != mPressedButtons.end())
			{
				mPressedButtons.erase(it);
				SafeForEach<IMouseListener>(mMouseListeners, [button] (IMouseListener* listener) {
					listener->OnMouseButtonReleased(button);
				});
			}
		}
	}
	
	void Window::OnMousePos(double xPos, double yPos)
	{
		mPrevMousePosX = mMousePosX;
		mPrevMousePosY = mMousePosY;
		mMousePosX = xPos;
		mMousePosY = yPos;
		SafeForEach<IMouseListener>(mMouseListeners, [xPos, yPos] (IMouseListener* listener) {
			listener->OnMouseMoved(xPos, yPos);
		});
	}
	
	void Window::OnSizeChanged(int width, int height)
	{
		mWindowWidth = width;
		mWindowHeight = height;
		UpdateViewport();
		UpdateAspectRatio();
		SafeForEach<IWindowSizeListener>(mWindowSizeListeners, [width, height] (IWindowSizeListener* listener) {
			listener->OnSizeChanged(width, height);
		});
	}
	
	void Window::OnSizeChanged()
	{
		glfwGetWindowSize(mWindow, &mWindowWidth, &mWindowHeight);
		OnSizeChanged(mWindowWidth, mWindowHeight);
	}
	
	void Window::AddListener(IKeyListener& listener)
	{
		mKeyListeners.insert(&listener);
	}
	
	void Window::RemoveListener(IKeyListener& listener)
	{
		auto it = mKeyListeners.find(&listener);
		if (it != mKeyListeners.end())
		{
			mKeyListeners.erase(it);
		}
	}
	
	bool Window::IsKeyDown(int keyCode) const
	{
		return (mPressedKeys.find(keyCode) != mPressedKeys.end());
	}
	
	void Window::AddListener(IMouseListener& listener)
	{
		mMouseListeners.insert(&listener);
	}
	
	void Window::RemoveListener(IMouseListener& listener)
	{
		auto it = mMouseListeners.find(&listener);
		if (it != mMouseListeners.end())
		{
			mMouseListeners.erase(it);
		}
	}
	
	bool Window::IsMouseButtonDown(int mouseButton) const
	{
		return (mPressedButtons.find(mouseButton) != mPressedButtons.end());
	}
	
	void Window::AddListener(IWindowSizeListener& listener)
	{
		mWindowSizeListeners.insert(&listener);
	}
	
	void Window::RemoveListener(IWindowSizeListener& listener)
	{
		auto it = mWindowSizeListeners.find(&listener);
		if (it != mWindowSizeListeners.end())
		{
			mWindowSizeListeners.erase(it);
		}
	}
	
	void Window::UpdateViewport()
	{
		int width, height;
		glfwGetFramebufferSize(mWindow, &width, &height);
		GLCALL(glViewport(0, 0, width, height));
	}

	void Window::UpdateAspectRatio()
	{
		mAspectRatio = static_cast<float>(mWindowWidth) / static_cast<float>(mWindowHeight);
	}
	
}
