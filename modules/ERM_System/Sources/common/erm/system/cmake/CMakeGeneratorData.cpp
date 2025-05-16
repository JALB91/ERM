#include "erm/system/cmake/CMakeGeneratorData.h"

#include <erm/log/Assert.h>

namespace erm {


const CMakeGeneratorData* getGeneratorDataFor(CMakeGenerator generator)
{
	const auto it = std::find_if(kAllGenerators.begin(), kAllGenerators.end(), [generator](const auto& generatorData) {
		return generatorData.mGenerator == generator;
	});
	
	if (!ERM_EXPECT(it != kAllGenerators.end(), "Invalid generator"))
	{
		return nullptr;
	}
	
	return &(*it);
}

}
