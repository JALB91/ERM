#include "erm/ERM_NN.h"

#include "erm/nn/NN.h"

#include <erm/modules_lib/ObjectRegistry.h>
#include <erm/system/Environment.h>

namespace erm {

bool ERM_NNImpl::init()
{
	return true;
}

bool ERM_NNImpl::deinit()
{
	return true;
}

int ERM_NNImpl::run(int argc, char** argv)
{
	erm::nn::NN nn;

	if (const auto* command = nn.getArgsParser().parseArgs(argc, argv); command != nullptr)
	{
		return command->execute();
	}

	return EXIT_FAILURE;
}

}
