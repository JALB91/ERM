#include "erm/assets/AssetsRepo.h"

namespace erm {

AssetsRepo::~AssetsRepo() = default;

void AssetsRepo::clearAll()
{
	forEachAssets([](auto& assets) {
		assets.clear();
	});
}

} // namespace erm
