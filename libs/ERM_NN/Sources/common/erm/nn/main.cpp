#include "erm/nn/NN.h"

using namespace erm;

int main(int argc, char** argv)
{
	erm::nn::NN nn;
	const auto* command = nn.getArgsParser().parseArgs(argc, argv);
	
	if (command != nullptr)
	{
		command->callback();
	}

	return EXIT_SUCCESS;
}
