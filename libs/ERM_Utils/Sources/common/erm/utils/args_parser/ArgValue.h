#pragma once

#include "erm/utils/StaticString.h"

#include <erm/math/Types.h>

#include <variant>

namespace erm {

using ArgValue = std::variant<str128, i64, bool>;

}