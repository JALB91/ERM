#include "erm/assets/AssetsRepo.h"

namespace erm {

void AssetsRepo::clear()
{
	for (auto& [id, pool] : mAssetsPoolMap)
	{
		pool->clear();
	}
}

} // namespace erm
