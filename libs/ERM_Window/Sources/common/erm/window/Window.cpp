#include "erm/window/Window.h"
#include "erm/window/IWindowListener.h"

#include <erm/utils/Profiler.h>
#include <erm/utils/Utils.h>
#ifdef ERM_OPENGL
#	include <erm/utils/GlUtils.h>
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
void safeForEach(const std::set<T*>& set, const std::function<void(T* ptr)>& func)
{
	std::set<T*> copy = set;
	for (T* t : copy)
	{
		if (t)
		{
			func(t);
		}
	}
}

#if defined(ERM_VULKAN)
constexpr const char* const kTitle = "ERM Vulkan";
#elif defined(ERM_OPENGL)
constexpr const char* const kTitle = "ERM OpenGL";
#elif defined(ERM_DX12)
constexpr const char* const kTitle = "ERM DX12";
#endif

constexpr float kImGuiSpaceUp = 0.0f;
constexpr float kImGuiSpaceDown = 0.3f;
constexpr float kImGuiSpaceLeft = 0.2f;
constexpr float kImGuiSpaceRight = 0.2f;

bool firstRefresh = true;

} // namespace

/*
	WINDOW CALLBACKS
*/
namespace erm::internal {

void focusCallback(GLFWwindow* window, int focused)
{
	if (focused)
		static_cast<erm::Window*>(glfwGetWindowUserPointer(window))->onFocus();
	else
		static_cast<erm::Window*>(glfwGetWindowUserPointer(window))->onFocusLost();
}

void onMouseButton(GLFWwindow* window, int button, int action, int mods)
{
	static_cast<erm::Window*>(glfwGetWindowUserPointer(window))->onMouseButton(button, action, mods);
}

void onMousePos(GLFWwindow* window, double xPos, double yPos)
{
	static_cast<erm::Window*>(glfwGetWindowUserPointer(window))->onMousePos(xPos, yPos);
}

void onKey(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
	static_cast<erm::Window*>(glfwGetWindowUserPointer(window))->onKey(key, scanCode, action, mods);
}

void onFrameBufferResize(GLFWwindow* window, int /*width*/, int /*height*/)
{
	static_cast<erm::Window*>(glfwGetWindowUserPointer(window))->onSizeChanged();
}

void onWindowSizeChanged(GLFWwindow* window, int /*width*/, int /*height*/)
{
	static_cast<erm::Window*>(glfwGetWindowUserPointer(window))->onSizeChanged();
}

void onRefresh(GLFWwindow* window)
{
	if (firstRefresh)
	{
		firstRefresh = false;
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		static_cast<erm::Window*>(glfwGetWindowUserPointer(window))->onSizeChanged();
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

bool Window::init()
{
	if (!glfwInit())
	{
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
#if defined(ERM_VULKAN) || defined(ERM_DX12)
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#elif defined(ERM_OPENGL)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	mWindow = glfwCreateWindow(mode->width, mode->height, kTitle, nullptr, nullptr);

	if (!mWindow)
	{
		return false;
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

	glfwSetWindowFocusCallback(mWindow, internal::focusCallback);
	glfwSetMouseButtonCallback(mWindow, internal::onMouseButton);
	glfwSetCursorPosCallback(mWindow, internal::onMousePos);
	glfwSetKeyCallback(mWindow, internal::onKey);
	glfwSetWindowRefreshCallback(mWindow, internal::onRefresh);
	glfwSetFramebufferSizeCallback(mWindow, internal::onFrameBufferResize);
	glfwSetWindowSizeCallback(mWindow, internal::onWindowSizeChanged);

	return true;
}

bool Window::shouldClose()
{
	return glfwWindowShouldClose(mWindow) || isKeyDown(KEY_ESCAPE);
}

void Window::update()
{
	ERM_PROFILE_FUNCTION();

	mPrevMousePosX = mMousePosX;
	mPrevMousePosY = mMousePosY;
	glfwGetCursorPos(mWindow, &mMousePosX, &mMousePosY);
}

void Window::render()
{
	ERM_PROFILE_FUNCTION();

	glfwSwapBuffers(mWindow);
}

void Window::postRender()
{
	ERM_PROFILE_FUNCTION();

	glfwPollEvents();
}

void Window::onKey(int key, int /*scanCode*/, int action, int /*mods*/)
{
	ERM_PROFILE_FUNCTION();

	if (action == GLFW_PRESS)
	{
		mPressedKeys.insert(key);
		safeForEach<IWindowListener>(mWindowListeners, [key](IWindowListener* listener) {
			listener->onKeyPressed(key);
		});
	}
	else if (action == GLFW_RELEASE)
	{
		auto it = mPressedKeys.find(key);
		if (it != mPressedKeys.end())
		{
			mPressedKeys.erase(it);
			safeForEach<IWindowListener>(mWindowListeners, [key](IWindowListener* listener) {
				listener->onKeyReleased(key);
			});
		}
	}
}

void Window::onMouseButton(int button, int action, int /*mods*/)
{
	ERM_PROFILE_FUNCTION();

	if (action == GLFW_PRESS)
	{
		mPressedButtons.insert(button);
		safeForEach<IWindowListener>(mWindowListeners, [button](IWindowListener* listener) {
			listener->onMouseButtonPressed(button);
		});
	}
	else if (action == GLFW_RELEASE)
	{
		auto it = mPressedButtons.find(button);
		if (it != mPressedButtons.end())
		{
			mPressedButtons.erase(it);
			safeForEach<IWindowListener>(mWindowListeners, [button](IWindowListener* listener) {
				listener->onMouseButtonReleased(button);
			});
		}
	}
}

void Window::onMousePos(double xPos, double yPos)
{
//	if (ImGui::GetIO().WantCaptureMouse)
//		return;

	safeForEach<IWindowListener>(mWindowListeners, [xPos, yPos](IWindowListener* listener) {
		listener->onMouseMoved(xPos, yPos);
	});
}

void Window::onSizeChanged()
{
	ERM_PROFILE_FUNCTION();

	glfwGetWindowSize(mWindow, &mWindowSize.x, &mWindowSize.y);
	glfwGetFramebufferSize(mWindow, &mFrameBufferSize.x, &mFrameBufferSize.y);
	updateViewport();
	updateAspectRatio();
	safeForEach<IWindowListener>(mWindowListeners, [this](IWindowListener* listener) {
		listener->onSizeChanged(mWindowSize.x, mWindowSize.y);
	});
}

void Window::onMaximised(bool /*wasMaximised*/)
{
	onSizeChanged();
}

void Window::onFocusLost()
{
	const bool hadFocus = hasFocus();
	IWindow::onFocusLost();

	if (hadFocus)
	{
		safeForEach<IWindowListener>(mWindowListeners, [](IWindowListener* listener) {
			listener->onFocusChanged();
		});
	}
}

void Window::onFocus()
{
	const bool hadFocus = hasFocus();
	IWindow::onFocus();

	if (!hadFocus)
	{
		safeForEach<IWindowListener>(mWindowListeners, [](IWindowListener* listener) {
			listener->onFocusChanged();
		});
	}

	int width, height;
	glfwGetWindowSize(mWindow, &width, &height);

	if (mWindowSize.x == width && mWindowSize.y == height)
	{
		return;
	}

	onSizeChanged();
}

BoundingBox2D Window::getNormalizedViewport() const
{
	return BoundingBox2D({0.0f, 0.0f}, {1.0f, 1.0f});
}

void Window::updateViewport()
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
	const vec2 size = frameBufferBounds.GetSize();

	GL_CALL(glViewport(
		static_cast<int>(frameBufferBounds.mMin.x),
		static_cast<int>(frameBufferBounds.mMin.y),
		static_cast<int>(size.x),
		static_cast<int>(size.y)));
#endif
}

void Window::updateAspectRatio()
{
	const auto& size = mViewport.getSize();
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
