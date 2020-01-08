#pragma once

#include "ecs/ECSConfig.h"

namespace erm {
	namespace ecs {
		
		struct EntityId
		{
		public:
			EntityId(ID id = ROOT_ID - 1)
				: mId(id)
			{}
			
			inline ID operator()() const { return mId; }
			inline bool operator==(EntityId other) const { return mId == other.mId; }
			
			inline bool IsValid() const
			{
				return mId >= ROOT_ID && mId < MAX_ID;
			}
			
			inline ID Get() const { return mId; }
			inline void Set(ID id) { mId = id; }
			inline void Reset() { mId = ROOT_ID - 1; }
			
		private:
			ID mId;
			
		};
		
	}
}
