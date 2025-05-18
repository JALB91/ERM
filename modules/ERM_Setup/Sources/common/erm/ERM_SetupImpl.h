#pragma once

namespace erm {

class ERM_SetupImpl
{
public:
	bool init() const;
	bool deinit() const;
	int run(int argc, char** argv) const;

};

}