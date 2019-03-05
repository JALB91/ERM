#pragma once

namespace erm {
	
	class IWindowSizeListener;
	
	class IWindowSizeProvider
	{
	public:
		virtual ~IWindowSizeProvider() = default;
		
		virtual void AddListener(IWindowSizeListener& listener) = 0;
		virtual void RemoveListener(IWindowSizeListener& listener) = 0;
		
		virtual int GetWindowWidth() const = 0;
		virtual int GetWindowHeight() const = 0;
		virtual float GetAspectRatio() const = 0;
		
	};
	
}
