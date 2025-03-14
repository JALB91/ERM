#pragma once

#include "erm/nn/IStatementHandler.h"
#include "erm/nn/Macros.h"

namespace erm::nn {

class StructStatementHandler : public IStatementHandler
{
public:
	StructStatementHandler(std::string_view name);
	virtual ~StructStatementHandler() = default;
	
	std::unique_ptr<IStatement> parse(const Program& program, size_t& tokenIndex) override;
	
};

}

ERM_NN_STATEMENT_HANDLER(erm::nn::StructStatementHandler, struct)
