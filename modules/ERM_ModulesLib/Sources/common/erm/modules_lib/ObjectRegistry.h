#pragma once

#include <erm/math/Types.h>

#include <erm/utils/TypeID.h>

#include <mutex>
#include <unordered_map>

namespace erm {

class ObjectRegistry
{
public:
	template<typename T>
	static void set(T& object)
	{
		auto& instance = getInstance();
		const auto lock = std::scoped_lock(instance.mMutex);
		const auto typeID = getID<T>();
		auto& objects = instance.mObjects;
		objects[typeID] = static_cast<void*>(&object);
	}
	
	template<typename T>
	static T* get()
	{
		auto& instance = getInstance();
		const auto lock = std::scoped_lock(instance.mMutex);
		auto& objects = instance.mObjects;
		const auto typeID = getID<T>();
		const auto it = objects.find(typeID);
		if (it == objects.end())
		{
			return nullptr;
		}
		return static_cast<T*>(it->second);
	}

	template<typename T>
	static T& require()
	{
		auto* result = get<T>();
		assert(result != nullptr);
		return *result;
	}
	
	template<typename T>
	static void remove()
	{
		auto& instance = getInstance();
		const auto lock = std::scoped_lock(instance.mMutex);
		auto& objects = instance.mObjects;
		const auto it = objects.find(getID<T>());
		if (it != objects.end()) 
		{
			objects.erase(it);
		}
	}
	
	static void clear()
	{
		auto& instance = getInstance();
		const auto lock = std::scoped_lock(instance.mMutex);
		instance.mObjects.clear();
	}

	static bool empty()
	{
		auto& instance = getInstance();
		const auto lock = std::scoped_lock(instance.mMutex);
		return instance.mObjects.empty();
	}
	
private:
	ObjectRegistry() = default;
	
	static ObjectRegistry& getInstance();
	
	std::unordered_map<u64, void*> mObjects;
	std::mutex mMutex;
	
};

}
