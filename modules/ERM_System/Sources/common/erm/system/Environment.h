#pragma once

#include <erm/modules_lib/IModuleObject.h>

#include <refl.hpp>

namespace erm {

class Environment : public IModuleObject<Environment>
{
public:
	const char* get(const char* key) const;
	int set(const char* key, const char* value) const;

};

}

REFL_AUTO(type(erm::Environment));
