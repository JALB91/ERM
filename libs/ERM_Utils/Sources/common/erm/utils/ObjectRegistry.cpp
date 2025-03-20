#include "erm/utils/ObjectRegistry.h"

namespace erm {

std::unordered_map<u64, std::shared_ptr<void>> ObjectRegistry::sObjects;
std::mutex ObjectRegistry::sMutex;

}
