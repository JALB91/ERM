#pragma once

namespace erm {
	
	class Renderer;
	
	class ISceneObject
	{
	public:
		virtual ~ISceneObject() = default;
		
		virtual void OnUpdate() = 0;
		virtual void OnImGuiRender() = 0;
		virtual void OnRender(const Renderer& renderer) = 0;
		
	};
	
}
