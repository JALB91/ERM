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
		auto& instance = getInstance();
		const auto lock = std::scoped_lock(instance.mMutex);
		const auto typeID = utils::getID<T>();
		auto& objects = instance.mObjects;
		ERM_ASSERT(objects.find(typeID) == objects.end(), "Overriding an object in the registry");
		objects[typeID] = std::static_pointer_cast<void>(object);
	}
	
	template<typename T>
	static std::shared_ptr<T> get()
	{
		auto& instance = getInstance();
		const auto lock = std::scoped_lock(instance.mMutex);
		auto& objects = instance.mObjects;
		const auto it = objects.find(utils::getID<T>());
		if (!ERM_EXPECT(it != objects.end(), "Attempt to retrieve uninitialized object from the registry"))
			return nullptr;
		return std::static_pointer_cast<T>(it->second);
	}
	
	template<typename T>
	static std::weak_ptr<T> getWeak()
	{
		auto& instance = getInstance();
		const auto lock = std::scoped_lock(instance.mMutex);
		auto& objects = instance.mObjects;
		const auto it = objects.find(utils::getID<T>());
		if (!ERM_EXPECT(it != objects.end(), "Attempt to retrieve uninitialized object from the registry"))
			return nullptr;
		return std::weak_ptr<T>(std::static_pointer_cast<T>(it->second));
	}
	
	template<typename T>
	static void remove()
	{
		auto& instance = getInstance();
		const auto lock = std::scoped_lock(instance.mMutex);
		auto& objects = instance.mObjects;
		const auto it = objects.find(utils::getID<T>());
		if (ERM_EXPECT(it != objects.end(), "Trying to remove uninitialized object from the registry")) {
			objects.erase(it);
		}
	}
	
	static void clear()
	{
		auto& instance = getInstance();
		const auto lock = std::scoped_lock(instance.mMutex);
		instance.mObjects.clear();
	}
	
private:
	ObjectRegistry() = default;
	
	static ObjectRegistry& getInstance();
	
	std::unordered_map<u64, std::shared_ptr<void>> mObjects;
	std::mutex mMutex;
	
};

}
