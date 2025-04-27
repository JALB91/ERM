#pragma once

#include <string>
#include <string_view>

namespace erm::fs {

void writeToFile(std::string_view path, std::string_view data);
std::string readFromFile(std::string_view path);

}