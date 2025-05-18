#pragma once

namespace erm {

class ERM_EditorImpl
{
public:
	bool init();
	bool deinit();
	int run(int argc, char** argv);

};

}