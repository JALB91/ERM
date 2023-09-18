#pragma once

#include <array>

namespace erm::nn {

constexpr std::array kStatements {
    "import",
    "struct",
    "enum"
};

constexpr std::array kTypes {
    "int",
    "float",
    "double",
    "long",
    "uint",
    "string"
};

}