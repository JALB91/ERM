#pragma once

@ERM_GENERATED_FILE_WARNING@

#if defined(ERM_NN_IMPL)
#error \"ERM_NN_IMPL already defined, this should not happen, did you include this file twice?\"
#endif

#define ERM_NN_IMPL

namespace erm::nn::internal {

static constexpr auto kERMNNCounterStart = __COUNTER__;

}

@NNSourcesIncludes@

namespace erm::nn::internal {

static constexpr auto kERMNNCounterEnd = __COUNTER__;

}

#define REGISTER_STATEMENT_HANDLERS(STATEMENT_HANDLERS_MAP) \
utils::for_constexpr<erm::nn::internal::kERMNNCounterStart + 1, erm::nn::internal::kERMNNCounterEnd - (erm::nn::internal::kERMNNCounterStart + 1)>([&] (const auto index) { \
    addStatementHandler<index.value>(STATEMENT_HANDLERS_MAP); \
});
