#pragma once

#include "input/Keys.h"

namespace erm {
	
	class IKeyListener;
	
	class IKeyInfoProvider
	{
	public:
		virtual ~IKeyInfoProvider() = default;
		
		virtual void AddListener(IKeyListener& listener) = 0;
		virtual void RemoveListener(IKeyListener& listener) = 0;
		
		virtual bool IsKeyDown(Key keyCode) const = 0;
		
	};
	
}
