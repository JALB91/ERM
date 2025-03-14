#pragma once

#include "erm/nn/IStatementHandler.h"
#include "erm/nn/Macros.h"

namespace erm::nn {

class VariableStatementHandler : public IStatementHandler
{
public:
	VariableStatementHandler(std::string_view name);
	virtual ~VariableStatementHandler() = default;
	
	std::unique_ptr<IStatement> parse(const Program& program, size_t& tokenIndex) override;
	
};

}

ERM_NN_STATEMENT_HANDLER(erm::nn::VariableStatementHandler, int)
ERM_NN_STATEMENT_HANDLER(erm::nn::VariableStatementHandler, uint)
ERM_NN_STATEMENT_HANDLER(erm::nn::VariableStatementHandler, float)
ERM_NN_STATEMENT_HANDLER(erm::nn::VariableStatementHandler, double)
ERM_NN_STATEMENT_HANDLER(erm::nn::VariableStatementHandler, long)
ERM_NN_STATEMENT_HANDLER(erm::nn::VariableStatementHandler, string)
