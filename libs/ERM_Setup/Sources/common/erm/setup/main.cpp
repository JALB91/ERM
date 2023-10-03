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
		erm::OptionalArg arg(erm::ArgValueType::BOOL, 'v', "verbose", false);
		arg.setDescription("Enable a more verbose output");
		parser->addOptionalArg(std::move(arg));
	}

	{
		erm::OptionalArg arg(erm::ArgValueType::STRING, 't', "target_api", "Vulkan");
		arg.setDescription("Set target API");
		parser->addOptionalArg(std::move(arg));
	}

	{
		erm::OptionalArg arg(erm::ArgValueType::BOOL, 'o', "open", false);
		arg.setDescription("Open the project after generating");
		parser->addOptionalArg(std::move(arg));
	}

	{
		erm::OptionalArg arg(erm::ArgValueType::BOOL, 'c', "compile", false);
		arg.setDescription("Compile the project after generating");
		parser->addOptionalArg(std::move(arg));
	}

	{
		erm::OptionalArg arg(erm::ArgValueType::BOOL, 'r', "run", false);
		arg.setDescription("Run the project after generating and compiling");
		parser->addOptionalArg(std::move(arg));
	}

	{
		erm::OptionalArg arg(erm::ArgValueType::BOOL, "rtx", false);
		arg.setDescription("Enable ray tracing");
		parser->addOptionalArg(std::move(arg));
	}

	{
		erm::OptionalArg arg(erm::ArgValueType::BOOL, 'i', "interactive", false);
		arg.setDescription("Run cmake in interactive mode");
		parser->addOptionalArg(std::move(arg));
	}

	{
		erm::OptionalArg arg(erm::ArgValueType::BOOL, "tracy", false);
		arg.setDescription("Enable tracy");
		parser->addOptionalArg(std::move(arg));
	}

	{
		erm::OptionalArg arg(erm::ArgValueType::BOOL, "trace", false);
		arg.setDescription("Trace cmake output");
		parser->addOptionalArg(std::move(arg));
	}

	{
		erm::PositionalArg arg("file", erm::ArgValueType::STRING, "Input file");
		parser->addPositionalArg(std::move(arg));
	}

	parser->setDescription("Setup ERM engine for development");
	parser->setCallback([](const erm::SubCommand& command) {
		
	});

	{
		auto& command = parser->addSubCommand("run");
		command.setDescription("Runs the program");
	}

	auto* command = parser.parseArgs(argc, argv);

	if (command != nullptr)
	{
		command->callback();
		return EXIT_SUCCESS;
	}

	return EXIT_FAILURE;
}