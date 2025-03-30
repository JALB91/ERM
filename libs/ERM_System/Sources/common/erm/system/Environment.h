#pragma once

#include <string_view>

namespace erm {

class Environment
{
public:
	Environment();
	
	std::string_view get(std::string_view key) const;

};

}
