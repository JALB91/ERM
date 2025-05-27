#pragma once

namespace erm {

class IObjectsPool
{	
public:
	IObjectsPool(const IObjectsPool&) = delete;
	IObjectsPool(IObjectsPool&&) = delete;
	
	IObjectsPool& operator=(const IObjectsPool&) = delete;
	IObjectsPool& operator=(IObjectsPool&&) = delete;
	
	virtual ~IObjectsPool() = default;
	
	virtual void clear() = 0;
	
protected:
	IObjectsPool() noexcept = default;
	
};

}
