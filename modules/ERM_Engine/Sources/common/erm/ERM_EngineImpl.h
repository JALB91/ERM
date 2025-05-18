#pragma once

#include "erm/engine/Engine.h"

namespace erm {

class ERM_EngineImpl
{
public:
	bool init();
	bool deinit();
	int run(int argc, char** argv);

private:
	Engine mEngine;

};

}