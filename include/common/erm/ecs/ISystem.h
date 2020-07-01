#pragma once

#include "erm/ecs/ECSConfig.h"
#include "erm/ecs/EntityId.h"

#include <array>
#include <memory>
#include <type_traits>
#include <utility>

namespace erm {
	class Renderer;
	namespace ecs {
		class ECS;
	}
} // namespace erm

namespace erm::ecs {

	template<
		typename T,
		typename Enable = std::enable_if_t<std::is_default_constructible_v<T>>>
	class ISystem
	{
	public:
		ISystem(ECS& ecs)
			: mECS(ecs)
			, mComponents {}
			, mComponentsBitmask {false}
		{}
		virtual ~ISystem() = default;

		virtual void Init() = 0;

		virtual void OnUpdate(float /*dt*/) {}
		virtual void OnPostUpdate() {}
		virtual void OnPreRender() {}
		virtual void OnRender() {}
		virtual void OnPostRender() {}

		virtual bool HasComponent(EntityId id) const final
		{
			return (id.IsValid() && mComponentsBitmask[id()]);
		}

		virtual const T* GetComponent(EntityId id) const final
		{
			return (HasComponent(id) ? &mComponents[id()] : nullptr);
		}

		virtual T* GetComponent(EntityId id) final
		{
			return (HasComponent(id) ? &mComponents[id()] : nullptr);
		}

		template<typename... Args>
		T* AddComponent(EntityId id, Args&&... args)
		{
			if (!HasComponent(id))
			{
				OnComponentBeingAdded(id);
				mComponentsBitmask[id()] = true;
				mComponents[id()] = T(std::forward<Args>(args)...);
				OnComponentAdded(id);
			}

			return GetComponent(id);
		}

		template<typename... Args>
		T* RequireComponent(EntityId id, Args&&... args)
		{
			return (HasComponent(id) ? GetComponent(id) : AddComponent(id, std::forward<Args>(args)...));
		}

		virtual void RemoveComponent(EntityId id) final
		{
			if (!HasComponent(id))
			{
				return;
			}

			OnComponentBeingRemoved(id);
			mComponentsBitmask[id()] = false;
			mComponents[id()] = {};
			OnComponentRemoved(id);
		}

		template<typename F>
		void ForEachComponent(F f)
		{
			for (ID i = 0; i < MAX_ID; ++i)
			{
				if (T* component = GetComponent(i))
				{
					f(*component);
				}
			}
		}

		template<typename F>
		void ForEachComponent(F f) const
		{
			for (ID i = 0; i < MAX_ID; ++i)
			{
				if (const T* component = GetComponent(i))
				{
					f(*component);
				}
			}
		}

		template<typename F>
		void ForEachComponentIndexed(F f)
		{
			for (ID i = 0; i < MAX_ID; ++i)
			{
				if (T* component = GetComponent(i))
				{
					f(*component, i);
				}
			}
		}

		template<typename F>
		void ForEachComponentIndexed(F f) const
		{
			for (ID i = 0; i < MAX_ID; ++i)
			{
				if (const T* component = GetComponent(i))
				{
					f(*component, i);
				}
			}
		}

	protected:
		virtual void OnComponentBeingAdded(EntityId /*id*/) {}
		virtual void OnComponentAdded(EntityId /*id*/) {}

		virtual void OnComponentBeingRemoved(EntityId /*id*/) {}
		virtual void OnComponentRemoved(EntityId /*id*/) {}

		ECS& mECS;
		std::array<T, MAX_ID> mComponents;
		std::array<bool, MAX_ID> mComponentsBitmask;
	};

} // namespace erm::ecs
