#pragma once

#include "erm/nn/IStatementHandler.h"

#include <erm/utils/assert/Assert.h>

#include <memory>
#include <string>
#include <unordered_map>

namespace erm::nn {

template<int N>
void addStatementHandler(std::unordered_map<std::string, std::unique_ptr<IStatementHandler>>& /*handlers*/)
{}

}

#if defined(ERM_NN_IMPL)
#define ERM_NN_STATEMENT_HANDLER(CLASS, NAME) \
namespace erm::nn { \
\
namespace internal { \
static constexpr auto k##NAME##NNStatementIndex = __COUNTER__; \
} \
\
template<> \
void addStatementHandler<internal::k##NAME##NNStatementIndex>(std::unordered_map<std::string, std::unique_ptr<IStatementHandler>>& handlers) \
{ \
	ERM_ASSERT_HARD(handlers.find(#NAME) == handlers.end()); \
	handlers[#NAME] = std::make_unique<CLASS>(#NAME); \
} \
\
}
#else
#define ERM_NN_STATEMENT_HANDLER(CLASS, NAME)
#endif
