#pragma once

#include "erm/ecs/ECSConfig.h"

namespace erm::ecs {

	struct EntityId
	{
	public:
		EntityId(ID id = INVALID_ID)
			: mId(id)
		{}

		inline ID operator()() const { return mId; }
		inline bool operator==(EntityId other) const { return mId == other.mId; }
		inline bool operator!=(EntityId other) const { return mId != other.mId; }
		inline bool operator<(EntityId other) const { return mId < other.mId; }

		inline bool IsValid() const
		{
			return mId >= ROOT_ID && mId < MAX_ID;
		}

		inline ID Get() const { return mId; }
		inline void Set(ID id) { mId = id; }
		inline void Reset() { mId = INVALID_ID; }

	private:
		ID mId;
	};

} // namespace erm::ecs
