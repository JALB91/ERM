#pragma once

#include "erm/assets/AssetHandle.h"

#include <erm/log/Assert.h>
#include <erm/modules_lib/IModuleObject.h>
#include <erm/utils/ObjectsPool.h>
#include <erm/utils/StringID.h>
#include <erm/utils/TypeID.h>

#include <refl.hpp>

#include <memory>
#include <unordered_map>

namespace erm {

class AssetsRepo : public IModuleObject<AssetsRepo>
{
public:
	void clear();
	
	template<typename T>
	[[nodiscard]] AssetHandle<T> load(std::unique_ptr<T> asset)
	{
		return getAssetsPool<T>().load(std::move(asset));
	}
	
	template<typename T>
	T* get(AssetHandle<T> handle)
	{
		return getAssetsPool<T>().get(handle);
	}
	
	template<typename T>
	bool unload(AssetHandle<T> handle)
	{
		return getAssetsPool<T>().unload(handle);
	}

private:
	template<typename T>
	ObjectsPool<T>& getAssetsPool()
	{
		const auto typeID = getID<ObjectsPool<T>>();
		const auto it = mAssetsPoolMap.find(typeID);
		
		if (it != mAssetsPoolMap.end())
		{
			return static_cast<ObjectsPool<T>&>(*it->second);
		}
		
		auto [iterator, result] = mAssetsPoolMap.emplace(typeID, std::make_unique<ObjectsPool<T>>());
		ERM_ASSERT(result);
		return static_cast<ObjectsPool<T>&>(*iterator->second);
	}
	
	std::unordered_map<u64, std::unique_ptr<IObjectsPool>> mAssetsPoolMap;
	
};

} // namespace erm

REFL_AUTO(type(erm::AssetsRepo));
