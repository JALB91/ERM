#pragma once

#include "erm/modules_lib/ObjectRegistry.h"

#include <erm/log/Assert.h>

#include <refl.hpp>

namespace erm {

template<typename T>
class IModuleObject
{
public:
	IModuleObject(const IModuleObject&) = delete;
	IModuleObject(IModuleObject&&) = delete;
	
	IModuleObject& operator=(const IModuleObject&) = delete;
	IModuleObject& operator=(IModuleObject&&) = delete;
	
protected:
	IModuleObject() noexcept
	{
		ERM_ASSERT(ObjectRegistry::get<T>() == nullptr);
		ObjectRegistry::set<T>(static_cast<T&>(*this));
	}
	
	virtual ~IModuleObject()
	{
		ERM_ASSERT(ObjectRegistry::get<T>() == this);
		ObjectRegistry::remove<T>();
	}

};

}
