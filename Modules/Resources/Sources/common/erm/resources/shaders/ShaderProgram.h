#pragma once

#include "erm/resources/data_structs/IAsset.h"

#include <string_view>

namespace erm {

class ShaderProgram : public IAsset
{
public:
	ShaderProgram(std::string_view path = "")
		: IAsset(path, path)
	{}
};

}

