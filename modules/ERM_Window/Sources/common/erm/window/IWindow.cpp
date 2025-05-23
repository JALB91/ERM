#include "erm/window/IWindow.h"

namespace erm {

IWindow::IWindow() noexcept
	: mMousePosX(0.0)
	, mPrevMousePosX(0.0)
	, mMousePosY(0.0)
	, mPrevMousePosY(0.0)
	, mFrameBufferSize()
	, mWindowSize()
	, mViewport({0.0f, 0.0f}, {0.0f, 0.0f})
	, mAspectRatio(0.0f)
	, mHasFocus(true)
{}

void IWindow::addListener(IWindowListener& listener)
{
	mWindowListeners.insert(&listener);
}

void IWindow::removeListener(IWindowListener& listener)
{
	auto it = mWindowListeners.find(&listener);
	if (it != mWindowListeners.end())
	{
		mWindowListeners.erase(it);
	}
}

void IWindow::onFocusLost()
{
	mHasFocus = false;
}

void IWindow::onFocus()
{
	mHasFocus = true;
}

} // namespace erm
