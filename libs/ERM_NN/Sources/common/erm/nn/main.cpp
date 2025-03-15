#include "erm/nn/NN.h"

using namespace erm;

int main(int argc, char** argv)
{
	erm::nn::NN nn;
	
	if (const auto* command = nn.getArgsParser().parseArgs(argc, argv); command != nullptr)
	{
		return command->execute();
	}

	return EXIT_FAILURE;
}
