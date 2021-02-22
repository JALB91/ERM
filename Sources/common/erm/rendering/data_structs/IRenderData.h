#pragma once

#include "erm/rendering/data_structs/UniformBufferObject.h"

#include <map>
#include <memory>

namespace erm {

	struct IRenderData
	{
		IRenderData() = default;
		virtual ~IRenderData() = default;

		IRenderData(IRenderData&& other)
			: mUbos(std::move(other.mUbos))
		{}

		IRenderData& operator=(IRenderData&& other)
		{
			mUbos = std::move(other.mUbos);
			return *this;
		}

		template<typename T>
		inline void SetUbo(T ubo)
		{
			if (mUbos.find(T::ID) == mUbos.end())
				mUbos[T::ID] = std::make_unique<T>();

			*static_cast<T*>(mUbos[T::ID].get()) = ubo;
		}

		inline bool HasUbo(UboId id) const
		{
			return mUbos.find(id) != mUbos.cend();
		}

		std::map<UboId, std::unique_ptr<IUbo>> mUbos;
	};

} // namespace erm