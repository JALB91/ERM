#pragma once

namespace erm {

class ERM_NNImpl
{
public:
	bool init();
	bool deinit();
	int run(int argc, char** argv);

};

}