#pragma once

namespace erm {
	
	class IWindowSizeListener
	{
	public:
		virtual ~IWindowSizeListener() = default;
		
		virtual void OnSizeChanged(int width, int height) = 0;
		
	};
	
}
