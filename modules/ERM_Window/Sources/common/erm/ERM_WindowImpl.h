#pragma once

#include "erm/window/Window.h"

namespace erm {

class ERM_WindowImpl
{
public:
	bool init();
	bool deinit();
	
private:
	Window mWindow;

};

}