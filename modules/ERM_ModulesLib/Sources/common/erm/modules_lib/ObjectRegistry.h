#pragma once

#include <erm/math/Types.h>

#include <erm/utils/TypeID.h>

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
		const auto typeID = getID<T>();
		auto& objects = instance.mObjects;
		objects[typeID] = std::static_pointer_cast<void>(object);
	}
	
	template<typename T>
	static std::shared_ptr<T> get()
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
		return std::static_pointer_cast<T>(it->second);
	}
	
	template<typename T>
	static std::weak_ptr<T> getWeak()
	{
		auto& instance = getInstance();
		const auto lock = std::scoped_lock(instance.mMutex);
		auto& objects = instance.mObjects;
		const auto it = objects.find(getID<T>());
		if (it == objects.end())
		{
			return nullptr;
		}
		return std::weak_ptr<T>(std::static_pointer_cast<T>(it->second));
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
	
	std::unordered_map<u64, std::shared_ptr<void>> mObjects;
	std::mutex mMutex;
	
};

}
