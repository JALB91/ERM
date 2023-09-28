#include "erm/nn/NN.h"
#include "erm/nn/Parser.h"

#include <erm/utils/log/Log.h>
#include <erm/utils/Macros.h>

#include <nlohmann/json.hpp>

#include <iostream>

using namespace erm;

int main(int argc, char** argv)
{
	ERM_UNUSED(argc);
	ERM_UNUSED(argv);
	// erm::nn::Parser parser;
	// const auto program = parser.parseProgram("", "");

	ERM_LOG_WARNING("NN not implemented yet");

	return EXIT_FAILURE;
}
