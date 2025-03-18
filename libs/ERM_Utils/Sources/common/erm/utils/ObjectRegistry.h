#pragma once

#include "erm/utils/assert/Assert.h"
#include "erm/utils/TypeID.h"

#include <erm/math/Types.h>

#include <memory>
#include <mutex>
#include <unordered_map>

namespace erm {

class ObjectRegistry
{
public:
	template<typename T>
	static void set(std::shared_ptr<T> object)
	{
		const auto lock = std::scoped_lock(sMutex);
		const auto typeID = utils::getID<T>();
		ERM_ASSERT(sObjects.find(typeID) == sObjects.end(), "Overriding an object in the registry");
		sObjects[typeID] = std::static_pointer_cast<void>(object);
	}
	
	template<typename T>
	static std::shared_ptr<T> get()
	{
		const auto lock = std::scoped_lock(sMutex);
		const auto it = sObjects.find(utils::getID<T>());
		if (!ERM_EXPECT(it != sObjects.end(), "Attempt to retrieve uninitialized object from the registry"))
			return nullptr;
		return std::static_pointer_cast<T>(it->second);
	}
	
	template<typename T>
	static std::weak_ptr<T> getWeak()
	{
		const auto lock = std::scoped_lock(sMutex);
		const auto it = sObjects.find(utils::getID<T>());
		if (!ERM_EXPECT(it != sObjects.end(), "Attempt to retrieve uninitialized object from the registry"))
			return nullptr;
		return std::weak_ptr<T>(std::static_pointer_cast<T>(it->second));
	}
	
	template<typename T>
	static void remove()
	{
		const auto lock = std::scoped_lock(sMutex);
		const auto it = sObjects.find(utils::getID<T>());
		if (ERM_EXPECT(it != sObjects.end(), "Trying to remove uninitialized object from the registry")) {
			sObjects.erase(it);
		}
	}
	
	static void clear()
	{
		const auto lock = std::scoped_lock(sMutex);
		sObjects.clear();
	}
	
private:
	ObjectRegistry() = default;
	
	static std::unordered_map<u64, std::shared_ptr<void>> sObjects;
	static std::mutex sMutex;
	
};

}
