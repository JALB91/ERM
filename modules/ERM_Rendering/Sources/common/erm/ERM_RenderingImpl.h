#pragma once

#include "erm/rendering/Device.h"
#include "erm/rendering/renderer/Renderer.h"

namespace erm {

class ERM_RenderingImpl
{
public:
	ERM_RenderingImpl();
	
	bool init();
	bool deinit();
	
private:
	Device mDevice;
	Renderer mRenderer;

};

}