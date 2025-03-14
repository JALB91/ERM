#pragma once

#include "erm/nn/IStatementHandler.h"
#include "erm/nn/Macros.h"

#include <refl.hpp>

namespace erm::nn {

class EnumStatementHandler : public IStatementHandler
{
public:
	EnumStatementHandler(std::string_view name);
	virtual ~EnumStatementHandler() = default;
	
	std::unique_ptr<IStatement> parse(const Program& program, size_t& tokenIndex) override;
	
};

}

ERM_NN_STATEMENT_HANDLER(erm::nn::EnumStatementHandler, enum)
