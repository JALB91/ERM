#include <iostream>

#include <erm/utils/assert/Assert.h>
#include <erm/utils/log/Log.h>
#include <erm/utils/args_parser/ArgsParser.h>
#include <erm/utils/Utils.h>

int main(int argc, char** argv)
{
	ERM_ASSERT(erm::utils::hasCommand("cmake"));

	erm::ArgsParser parser;
	
	{
		erm::OptionalArg arg('v', "verbose", false, "Enable a more verbose output");
		parser->addOptionalArg(std::move(arg));
	}

	{
		erm::OptionalArg arg('t', "target_api", "Vulkan", "Set target API");
		parser->addOptionalArg(std::move(arg));
	}

	{
		erm::OptionalArg arg('o', "open", false, "Open the project after generating");
		parser->addOptionalArg(std::move(arg));
	}

	{
		erm::OptionalArg arg('c', "compile", false, "Compile the project after generating");
		parser->addOptionalArg(std::move(arg));
	}

	{
		erm::OptionalArg arg('r', "run", false, "Run the project after generating and compiling");
		parser->addOptionalArg(std::move(arg));
	}

	{
		erm::OptionalArg arg(std::nullopt, "rtx", false, "Enable ray tracing");
		parser->addOptionalArg(std::move(arg));
	}

	{
		erm::OptionalArg arg('i', "interactive", false, "Run cmake in interactive mode");
		parser->addOptionalArg(std::move(arg));
	}

	{
		erm::OptionalArg arg(std::nullopt, "tracy", false, "Enable tracy");
		parser->addOptionalArg(std::move(arg));
	}

	{
		erm::OptionalArg arg(std::nullopt, "trace", false, "Trace cmake output");
		parser->addOptionalArg(std::move(arg));
	}

	parser->setDescription("Setup ERM engine for development");
	parser->setCallback([](const erm::SubCommand& command) {
		const auto& optArgs = command.getOptionalArgs();


	});

	auto* command = parser.parseArgs(argc, argv);

	if (command != nullptr)
	{
		command->callback();
		return EXIT_SUCCESS;
	}

	return EXIT_FAILURE;
}