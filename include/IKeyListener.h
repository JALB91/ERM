#pragma once

namespace erm {
	
	class IKeyListener
	{
	public:
		virtual ~IKeyListener() = default;
		
		virtual void OnKeyPressed(int keyCode) = 0;
		virtual void OnKeyReleased(int keyCode) = 0;
		
	};
	
}
