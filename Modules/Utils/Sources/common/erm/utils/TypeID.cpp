#include "erm/utils/TypeID.h"

namespace erm::utils::internal {

size_t GetNextID()
{
	static size_t sNext = 0;
	return sNext++;
}

}

