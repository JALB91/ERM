#include "erm/utils/TypeID.h"

namespace erm::utils::internal {

u64 getNextID()
{
	static u64 sNext = 0;
	return sNext++;
}

}

