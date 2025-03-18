#include "erm/nn/ImportStatementHandler.h"

#include "erm/nn/Program.h"

namespace erm::nn {

struct ImportStatement : public IStatement
{
	std::string mPath;
	
	void print() const override
	{
		ERM_LOG("Import\n\tPath: %s", mPath.c_str());
	}
};

ImportStatementHandler::ImportStatementHandler(std::string_view name)
	: IStatementHandler(name)
{}

std::unique_ptr<IStatement> ImportStatementHandler::parse(const Program& program, size_t& tokenIndex)
{
	const auto& tokens = program.getTokens();
	const auto& token = tokens[tokenIndex];
	
	ERM_ASSERT_HARD(token.mType == TokenType::IDENTIFIER && token.mValue == mName);
	
	if (!ERM_EXPECT(tokens.size() > ++tokenIndex, "Invalid EOF"))
	{
		return nullptr;
	}
	
	const auto& path = tokens[tokenIndex];
	if (!ERM_EXPECT(path.mType == TokenType::STRING_LITERAL, "Invalid token after import declaration, expected path"))
	{
		return nullptr;
	}
	
	ImportStatement importStm;
	importStm.mPath = path.mValue;
	
	if (!ERM_EXPECT(tokens.size() > ++tokenIndex, "Invalid EOF"))
	{
		return nullptr;
	}
	
	const auto& sc = tokens[tokenIndex];
	if (!ERM_EXPECT(sc.mType == TokenType::SYMBOL && sc.mValue == ";", "Invalid token after import path, expected \";\""))
	{
		return nullptr;
	}
	
	return std::make_unique<ImportStatement>(std::move(importStm));
}

}
