#pragma once

#include <refl.hpp>

namespace erm::nn {

struct IStatement
{
	virtual ~IStatement() = default;
	
	virtual void print() const = 0;
};

}

REFL_AUTO(type(erm::nn::IStatement));
