#pragma once

#include "erm/nn/IStatementHandler.h"
#include "erm/nn/Macros.h"

namespace erm::nn {

class ImportStatementHandler : public IStatementHandler
{
public:
	ImportStatementHandler(std::string_view name);
	virtual ~ImportStatementHandler() = default;
	
	std::unique_ptr<IStatement> parse(const Program& program, size_t& tokenIndex) override;
	
};

}

ERM_NN_STATEMENT_HANDLER(erm::nn::ImportStatementHandler, import)
