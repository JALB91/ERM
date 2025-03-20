#pragma once

#include "erm/nn/IStatement.h"

#include <erm/utils/StaticString.h>

#include <memory>
#include <string_view>

namespace erm::nn {
class Program;
}

namespace erm::nn {

class IStatementHandler
{
public:
	IStatementHandler(std::string_view name);
	virtual ~IStatementHandler() = default;
	
	virtual std::unique_ptr<IStatement> parse(const Program& program, size_t& tokenIndex) = 0;
	
protected:
	const str128 mName;
	
};

}
