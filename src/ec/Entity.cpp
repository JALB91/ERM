#include "ec/Entity.h"
#include "ec/components/TransformComponent.h"

#include "rendering/Renderer.h"

#include "utils/Utils.h"

#include <imgui.h>

namespace {
	
	static erm::Entity::EntityID nextEntityID = 0;
	
}

namespace erm {
	
	Entity::Entity(Game& game, const std::string& name /* = "" */)
		: mGame(game)
		, mName(name)
		, mEntityID(nextEntityID++)
		, mComponents{nullptr}
		, mComponentIDs{false}
		, mParent(nullptr)
		, mChildren()
		, mIsDirty(false)
	{
		if (mName.empty())
		{
			mName.append("Entity").append(std::to_string(mEntityID));
		}
		RequireComponent<TransformComponent>();
	}
	
	Entity::~Entity()
	{
		RemoveFromParent();
	}
	
	void Entity::OnUpdate(float dt)
	{
		ForEachComponent([dt](IComponent& component) {
			component.OnUpdate(dt);
		});
		ForEachChild([dt](Entity& child) {
			child.OnUpdate(dt);
		});
	}
	
	void Entity::OnPostUpdate()
	{
		ForEachComponent([](IComponent& component) {
			component.OnPostUpdate();
		});
		ForEachChild([](Entity& child) {
			child.OnPostUpdate();
		});
		SetDirty(false);
	}
	
	void Entity::OnRender()
	{
		ForEachComponent([](IComponent& component) {
			component.OnRender();
		});
		ForEachChild([](Entity& child) {
			child.OnRender();
		});
		
		mGame.GetRenderer().AddToQueue(*this);
	}
	
	void Entity::AddChild(Entity* child)
	{
		if (!EXPECT(child && child->GetParent() != this, "Trying to add nullptr as child")) return;
		
		if (child->GetParent())
		{
			child->RemoveFromParent();
		}
		else
		{
			child->SetDirty(true);
		}
		
		child->mParent = this;
		mChildren.emplace_back(child);
	}
	
	void Entity::RemoveFromParent()
	{
		if (mParent)
		{
			std::vector<Entity*>& children = mParent->mChildren;
			auto it = std::find(children.begin(), children.end(), this);
			if (it != children.end())
			{
				children.erase(it);
				SetDirty(true);
			}
		}
	}
	
	void Entity::SetDirty(bool isDirty)
	{
		ForEachComponent([isDirty](IComponent& component) {
			component.SetDirty(isDirty);
		});
		ForEachChild([isDirty](Entity& child) {
			child.SetDirty(isDirty);
		});
		mIsDirty = isDirty;
	}
	
	void Entity::ForEachComponent(const std::function<void(IComponent&)>& function)
	{
		for (ComponentID i = 0; i < kMaxComponentsNumber; ++i)
		{
			if (mComponentIDs.at(i))
			{
				function(*mComponents.at(i));
			}
		}
	}
	
	void Entity::ForEachChild(const std::function<void(Entity&)>& function)
	{
		for (Entity* entity: mChildren)
		{
			if (entity) function(*entity);
		}
	}
	
}
