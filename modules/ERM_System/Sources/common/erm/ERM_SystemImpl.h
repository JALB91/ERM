#pragma once

#include "erm/system/Environment.h"

namespace erm {

class ERM_SystemImpl
{
public:
	bool init();
	bool deinit();

private:
	Environment mEnvironment;

};

} // namespace erm