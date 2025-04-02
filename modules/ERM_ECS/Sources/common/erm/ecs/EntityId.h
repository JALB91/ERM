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

	inline bool isValid() const
	{
		return mId >= ROOT_ID && mId < MAX_ID;
	}

	inline ID get() const { return mId; }
	inline void set(ID id) { mId = id; }
	inline void reset() { mId = INVALID_ID; }

private:
	ID mId;
};

} // namespace erm::ecs
