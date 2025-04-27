#include "erm/ERM_NN.h"

#include "erm/nn/NN.h"

#include <erm/system/Environment.h>

#include <erm/modules/ObjectRegistry.h>

namespace erm {

bool ERM_NN::Impl::init() const
{
	return true;
}

bool ERM_NN::Impl::deinit() const
{
	return true;
}

int ERM_NN::Impl::run(int argc, char** argv) const
{
	erm::nn::NN nn;

	if (const auto* command = nn.getArgsParser().parseArgs(argc, argv); command != nullptr)
	{
		return command->execute();
	}

	return EXIT_FAILURE;
}

}
