#pragma once

#include "erm/nn/IStatementHandler.h"
#include "erm/nn/Macros.h"

namespace erm::nn {

class ClassStatementHandler : public IStatementHandler
{
public:
	ClassStatementHandler(std::string_view name);
	virtual ~ClassStatementHandler() = default;
	
	std::unique_ptr<IStatement> parse(const Program& program, size_t& tokenIndex) override;
	
};

}

ERM_NN_STATEMENT_HANDLER(ClassStatementHandler, class)
