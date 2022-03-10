#include "erm/rendering/window/Window.h"
#include "erm/rendering/window/IWindowListener.h"

#include "erm/utils/Profiler.h"
#include "erm/utils/Utils.h"
#ifdef ERM_OPENGL
#	include "erm/utils/GlUtils.h"
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>

#ifdef ERM_OPENGL
#	include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <algorithm>
#include <array>
#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <string>

/*
	WINDOW STUFF
*/
namespace {

template<typename T>
void SafeForEach(const std::set<T*>& set, const std::function<void(T* ptr)>& func)
{
	std::set<T*> copy = set;
	for (T* t : copy)
	{
		if (t)
			func(t);
	}
}

#if defined(ERM_VULKAN)
const char* const kTitle = "ERM Vulkan";
#elif defined(ERM_OPENGL)
const char* const kTitle = "ERM OpenGl";
#elif defined(ERM_DX12)
const char* const kTitle = "ERM DX12";
#endif

const float kImGuiSpaceUp = 0.0f;
const float kImGuiSpaceDown = 0.3f;
const float kImGuiSpaceLeft = 0.2f;
const float kImGuiSpaceRight = 0.2f;

bool firstRefresh = true;

} // namespace

/*
	WINDOW CALLBACKS
*/
namespace internal {

void FocusCallback(GLFWwindow* window, int focused)
{
	if (focused)
		static_cast<erm::Window*>(glfwGetWindowUserPointer(window))->OnFocus();
	else
		static_cast<erm::Window*>(glfwGetWindowUserPointer(window))->OnFocusLost();
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

void OnFrameBufferResize(GLFWwindow* window, int /*width*/, int /*height*/)
{
	static_cast<erm::Window*>(glfwGetWindowUserPointer(window))->OnSizeChanged();
}

void OnWindowSizeChanged(GLFWwindow* window, int /*width*/, int /*height*/)
{
	static_cast<erm::Window*>(glfwGetWindowUserPointer(window))->OnSizeChanged();
}

void OnRefresh(GLFWwindow* window)
{
	if (firstRefresh)
	{
		firstRefresh = false;
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		static_cast<erm::Window*>(glfwGetWindowUserPointer(window))->OnSizeChanged();
	}
}

} // namespace internal

/*
	WINDOW IMPL
*/
namespace erm {

Window::Window()
	: IWindow()
	, mWindow(nullptr)
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
#if defined(ERM_VULKAN) || defined(ERM_DX12)
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#elif defined(ERM_OPENGL)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	mWindow = glfwCreateWindow(mode->width, mode->height, kTitle, nullptr, nullptr);

	if (!mWindow)
	{
		throw std::runtime_error("GLFW Window creation failed!");
	}

	glfwMakeContextCurrent(mWindow);
	glfwSwapInterval(1);

#ifdef ERM_OPENGL
	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW initialization failed" << std::endl;
		glfwTerminate();
		return false;
	}
#endif

	glfwSetWindowUserPointer(mWindow, this);

	glfwSetWindowFocusCallback(mWindow, internal::FocusCallback);
	glfwSetMouseButtonCallback(mWindow, internal::OnMouseButton);
	glfwSetCursorPosCallback(mWindow, internal::OnMousePos);
	glfwSetKeyCallback(mWindow, internal::OnKey);
	glfwSetWindowRefreshCallback(mWindow, internal::OnRefresh);
	glfwSetFramebufferSizeCallback(mWindow, internal::OnFrameBufferResize);
	glfwSetWindowSizeCallback(mWindow, internal::OnWindowSizeChanged);

	return true;
}

bool Window::ShouldClose()
{
	return glfwWindowShouldClose(mWindow) || IsKeyDown(KEY_ESCAPE);
}

void Window::OnUpdate()
{
	ERM_PROFILE_FUNCTION();

	mPrevMousePosX = mMousePosX;
	mPrevMousePosY = mMousePosY;
	glfwGetCursorPos(mWindow, &mMousePosX, &mMousePosY);
}

void Window::OnRender()
{
	ERM_PROFILE_FUNCTION();

	glfwSwapBuffers(mWindow);
}

void Window::OnPostRender()
{
	ERM_PROFILE_FUNCTION();

	glfwPollEvents();
}

void Window::OnKey(int key, int /*scanCode*/, int action, int /*mods*/)
{
	ERM_PROFILE_FUNCTION();

	if (action == GLFW_PRESS)
	{
		mPressedKeys.insert(key);
		SafeForEach<IWindowListener>(mWindowListeners, [key](IWindowListener* listener) {
			listener->OnKeyPressed(key);
		});
	}
	else if (action == GLFW_RELEASE)
	{
		auto it = mPressedKeys.find(key);
		if (it != mPressedKeys.end())
		{
			mPressedKeys.erase(it);
			SafeForEach<IWindowListener>(mWindowListeners, [key](IWindowListener* listener) {
				listener->OnKeyReleased(key);
			});
		}
	}
}

void Window::OnMouseButton(int button, int action, int /*mods*/)
{
	ERM_PROFILE_FUNCTION();

	if (action == GLFW_PRESS)
	{
		mPressedButtons.insert(button);
		SafeForEach<IWindowListener>(mWindowListeners, [button](IWindowListener* listener) {
			listener->OnMouseButtonPressed(button);
		});
	}
	else if (action == GLFW_RELEASE)
	{
		auto it = mPressedButtons.find(button);
		if (it != mPressedButtons.end())
		{
			mPressedButtons.erase(it);
			SafeForEach<IWindowListener>(mWindowListeners, [button](IWindowListener* listener) {
				listener->OnMouseButtonReleased(button);
			});
		}
	}
}

void Window::OnMousePos(double xPos, double yPos)
{
	if (ImGui::GetIO().WantCaptureMouse)
		return;

	SafeForEach<IWindowListener>(mWindowListeners, [xPos, yPos](IWindowListener* listener) {
		listener->OnMouseMoved(xPos, yPos);
	});
}

void Window::OnSizeChanged()
{
	ERM_PROFILE_FUNCTION();

	glfwGetWindowSize(mWindow, &mWindowSize.x, &mWindowSize.y);
	glfwGetFramebufferSize(mWindow, &mFrameBufferSize.x, &mFrameBufferSize.y);
	UpdateViewport();
	UpdateAspectRatio();
	SafeForEach<IWindowListener>(mWindowListeners, [this](IWindowListener* listener) {
		listener->OnSizeChanged(mWindowSize.x, mWindowSize.y);
	});
}

void Window::OnMaximised(bool /*wasMaximised*/)
{
	OnSizeChanged();
}

void Window::OnFocusLost()
{
	const bool hadFocus = HasFocus();
	IWindow::OnFocusLost();

	if (hadFocus)
	{
		SafeForEach<IWindowListener>(mWindowListeners, [](IWindowListener* listener) {
			listener->OnFocusChanged();
		});
	}
}

void Window::OnFocus()
{
	const bool hadFocus = HasFocus();
	IWindow::OnFocus();

	if (!hadFocus)
	{
		SafeForEach<IWindowListener>(mWindowListeners, [](IWindowListener* listener) {
			listener->OnFocusChanged();
		});
	}

	int width, height;
	glfwGetWindowSize(mWindow, &width, &height);

	if (mWindowSize.x == width && mWindowSize.y == height)
		return;

	OnSizeChanged();
}

BoundingBox2D Window::GetNormalizedViewport() const
{
	return BoundingBox2D({0.0f, 0.0f}, {1.0f, 1.0f});
}

void Window::UpdateViewport()
{
	mViewport.mMin.x = mWindowSize.x * kImGuiSpaceLeft;
	mViewport.mMin.y = mWindowSize.y * kImGuiSpaceDown;
	mViewport.mMax.x = mWindowSize.x - (mWindowSize.x * kImGuiSpaceRight);
	mViewport.mMax.y = mWindowSize.y - (mWindowSize.y * kImGuiSpaceUp);
#if defined(ERM_OPENGL)
	BoundingBox2D frameBufferBounds;
	frameBufferBounds.mMin.x = mFrameBufferSize.x * kImGuiSpaceLeft;
	frameBufferBounds.mMin.y = mFrameBufferSize.y * kImGuiSpaceDown;
	frameBufferBounds.mMax.x = mFrameBufferSize.x - (mFrameBufferSize.x * kImGuiSpaceRight);
	frameBufferBounds.mMax.y = mFrameBufferSize.y - (mFrameBufferSize.y * kImGuiSpaceUp);
	const math::vec2 size = frameBufferBounds.GetSize();

	GL_CALL(glViewport(
		static_cast<int>(frameBufferBounds.mMin.x),
		static_cast<int>(frameBufferBounds.mMin.y),
		static_cast<int>(size.x),
		static_cast<int>(size.y)));
#endif
}

void Window::UpdateAspectRatio()
{
	const math::vec2 size = mViewport.GetSize();
	if (size.y > 0.0f)
	{
		mAspectRatio = size.x / size.y;
	}
	else
	{
		mAspectRatio = 0.0f;
	}
}

} // namespace erm
