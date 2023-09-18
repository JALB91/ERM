#include "erm/window/IWindow.h"

namespace erm {

IWindow::IWindow()
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

void IWindow::AddListener(IWindowListener& listener)
{
	mWindowListeners.insert(&listener);
}

void IWindow::RemoveListener(IWindowListener& listener)
{
	auto it = mWindowListeners.find(&listener);
	if (it != mWindowListeners.end())
	{
		mWindowListeners.erase(it);
	}
}

void IWindow::OnFocusLost()
{
	mHasFocus = false;
}

void IWindow::OnFocus()
{
	mHasFocus = true;
}

} // namespace erm