#include "erm/utils/TypeID.h"

namespace erm::utils::internal {

u64 GetNextID()
{
	static u64 sNext = 0;
	return sNext++;
}

}

