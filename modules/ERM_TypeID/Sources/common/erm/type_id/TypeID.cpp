#include "erm/type_id/TypeID.h"

namespace erm::internal {

u64 getNextID()
{
	static u64 sNext = 0;
	return sNext++;
}

}

