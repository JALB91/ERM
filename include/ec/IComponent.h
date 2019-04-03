#pragma once

namespace erm {
	
	class Entity;
	class Renderer;
	
	class IComponent
	{
	public:
		friend class Entity;
		
	public:
		IComponent(Entity& entity)
			: mEntity(entity)
			, mIsDirty(false)
		{}
		
		virtual ~IComponent() = default;
		
		virtual inline void OnUpdate(float dt) {}
		virtual inline void OnPostUpdate() {}
		virtual inline void OnImGuiRender() {}
		virtual inline void OnRender(const Renderer& renderer) {}
		
		inline Entity& GetEntity() { return mEntity; }
		inline const Entity& GetEntity() const { return mEntity; }
		
		inline void SetDirty() { mIsDirty = true; }
		inline bool IsDirty() const { return mIsDirty; }
		
	protected:
		Entity& mEntity;
		bool mIsDirty;
		
	};
	
}
