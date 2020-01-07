#pragma once

#include "ec/IComponent.h"
#include "game/Game.h"

#include <array>
#include <vector>
#include <memory>
#include <string>
#include <functional>

namespace erm {
	
	class Game;
	class Window;
	class Renderer;
	
	class Entity
	{
	private:
		static constexpr unsigned int kMaxComponentsNumber = 40;
		
		typedef unsigned int ComponentID;
		typedef std::array<bool, kMaxComponentsNumber> ComponentIDs;
		typedef std::array<std::unique_ptr<IComponent>, kMaxComponentsNumber> Components;
		
		static ComponentID GetNextComponentID()
		{
			static ComponentID nextID = 0;
			return nextID++;
		}
		
		template<typename T>
		static ComponentID GetComponentID()
		{
			static ComponentID nextID = GetNextComponentID();
			return nextID;
		}
		
	public:
		typedef unsigned int EntityID;
		
		Entity(Game& game, const std::string& name = "");
		~Entity();
		
		Entity(Entity&&) = delete;
		Entity(const Entity&) = delete;
		
		Entity& operator=(Entity&&) = delete;
		Entity& operator=(const Entity&) = delete;
		
		template<typename T>
		inline bool HasComponent() const
		{
			return mComponentIDs.at(Entity::GetComponentID<T>());
		}
		
		template<typename T>
		inline T* GetComponent() const
		{
			if (!HasComponent<T>()) return nullptr;
			return static_cast<T*>(mComponents.at(GetComponentID<T>()).get());
		}
		
		template<typename T, typename... Args>
		inline T& RequireComponent(Args&&... args)
		{
			if (HasComponent<T>()) return static_cast<T&>(*mComponents.at(GetComponentID<T>()));
			
			const ComponentID componentID = GetComponentID<T>();
			
			mComponents[componentID] = std::make_unique<T>(*this, std::move(args)...);
			mComponentIDs[componentID] = true;
			
			return static_cast<T&>(*mComponents.at(componentID));
		}
		
		void OnUpdate(float dt);
		void OnPostUpdate();
		void OnRender();
		
		inline Game& GetGame() { return mGame; }
		inline const Game& GetGame() const { return mGame; }
		
		inline const std::string& GetName() const { return mName; }
		inline void SetName(const std::string& name) { mName = name; }
		
		inline EntityID GetEntityID() const { return mEntityID; }
		
		void RemoveFromParent();
		void AddChild(Entity* child);
		inline Entity* GetParent() const { return mParent; }
		inline const std::vector<Entity*>& GetChildren() const { return mChildren; }
		
		void SetDirty(bool isDirty);
		inline bool IsDirty() const { return mIsDirty; }
		
		void ForEachComponent(const std::function<void(IComponent&)>& function);
		void ForEachChild(const std::function<void(Entity&)>& function);
		
	private:
		Game& mGame;
		std::string mName;
		EntityID mEntityID;
		Components mComponents;
		ComponentIDs mComponentIDs;
		
		Entity* mParent;
		std::vector<Entity*> mChildren;
		
		bool mIsDirty;
		
	};
	
}
