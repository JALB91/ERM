#include "erm/nn/Program.h"

#include "erm/nn/NNAllStatementHandlers.h"
#include "erm/nn/Tokenizer.h"

#include <erm/log/Assert.h>

#include <erm/utils/Utils.h>

#include <magic_enum/magic_enum.hpp>

#include <stack>

namespace erm::nn {

Program::Program(std::string_view name) noexcept
	: mName(name)
{
	REGISTER_STATEMENT_HANDLERS(mStatementHandlers);
}

bool Program::parse(std::string_view text)
{
	Tokenizer tokenizer(text);

	auto lookahead = tokenizer.getNextToken();
	
	while (lookahead.has_value())
	{
		if (lookahead->mType == TokenType::INVALID)
		{
			ERM_LOG_ERROR(
				"Invalid token found \"%s\", while parsing \"%s\" at line %d, %d",
				lookahead->mValue.data(),
				mName.data(),
				lookahead->mLine,
				lookahead->mLineOffset);
			mTokens.clear();
			
			return false;
		}
		
		mTokens.emplace_back(std::move(*lookahead));
		
		lookahead = tokenizer.getNextToken();
	}
	
	for (size_t i = 0; i < mTokens.size();)
	{
		const auto& token = mTokens[i];
		
		if (token.mType == TokenType::INVALID)
		{
			ERM_LOG_ERROR(
				"Invalid token found \"%s\" (%s), while parsing \"%s\" at line %d, %d",
				token.mValue.data(),
				magic_enum::enum_name(token.mType).data(),
				mName.data(),
				token.mLine,
				token.mLineOffset);
			return false;
		}
		
		const auto it = mStatementHandlers.find(token.mValue.data());
		ERM_ASSERT_HARD(it != mStatementHandlers.end());

		const auto& [_, handler] = *it;
		
		const auto statement = handler->parse(*this, i);

		if (!statement)
		{
			ERM_LOG_ERROR("Error while parsing \"%s\"", mName.data());
			return false;
		}
		
		statement->print();
	}
	
	return true;
}

void Program::debugPrint() const
{
	
}

}
