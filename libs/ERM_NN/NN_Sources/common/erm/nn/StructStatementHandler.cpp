#include "erm/nn/StructStatementHandler.h"

#include "erm/nn/Program.h"

namespace erm::nn {

StructStatementHandler::StructStatementHandler(std::string_view name)
	: IStatementHandler(name)
{}

std::unique_ptr<IStatement> StructStatementHandler::parse(const Program& /*program*/, size_t& /*tokenIndex*/)
{
	return nullptr;
}

}

