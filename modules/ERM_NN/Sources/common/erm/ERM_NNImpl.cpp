#include "erm/ERM_NN.h"

#include "erm/nn/NN.h"

namespace erm {

ERM_NN::ERM_NN() noexcept = default;

int ERM_NN::run(int argc, char** argv)
{
	erm::nn::NN nn;

	if (const auto* command = nn.getArgsParser().parseArgs(argc, argv); command != nullptr)
	{
		return command->execute();
	}

	return EXIT_FAILURE;
}

}
