#include "erm/assets/AssetsRepo.h"

namespace erm {

void AssetsRepo::clearAll()
{
	forEachAssets([](auto& assets) {
		assets.clear();
	});
}

} // namespace erm
