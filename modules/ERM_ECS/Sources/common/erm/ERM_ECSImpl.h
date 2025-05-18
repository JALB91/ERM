#pragma once

#include "erm/ecs/ECS.h"

namespace erm {

class ERM_ECSImpl
{
public:
	bool init();
	bool deinit();

private:
	ecs::ECS mECS;
	
};

}