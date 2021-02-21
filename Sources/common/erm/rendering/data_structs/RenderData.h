#pragma once

#include "erm/rendering/data_structs/RenderConfigs.h"
#include "erm/rendering/data_structs/UniformBufferObject.h"

#include <map>
#include <memory>
#include <optional>
#include <vector>

namespace erm {
	class Mesh;
}

namespace erm {

	struct RenderData
	{
		RenderData(const RenderConfigs& renderConfigs)
			: mRenderConfigs(renderConfigs)
		{}
		RenderData(const SubpassData& subpassData)
			: mRenderConfigs(subpassData)
		{}

		RenderData(const RenderData&) = delete;
		RenderData& operator=(const RenderData&) = delete;

		RenderData(RenderData&& other)
			: mRenderConfigs(other.mRenderConfigs)
			, mUbos(std::move(other.mUbos))
			, mMeshes(std::move(other.mMeshes))
		{}

		RenderData& operator=(RenderData&& other)
		{
			mRenderConfigs = other.mRenderConfigs;
			mUbos = std::move(other.mUbos);
			mMeshes = std::move(other.mMeshes);

			return *this;
		}

		template<typename T>
		inline void SetUbo(T ubo)
		{
			if (mUbos.find(T::ID) == mUbos.end())
				mUbos[T::ID] = std::make_unique<T>();

			*static_cast<T*>(mUbos[T::ID].get()) = ubo;
		}

		inline bool HasMesh(const Mesh* mesh) const
		{
			return std::find(mMeshes.cbegin(), mMeshes.cend(), mesh) != mMeshes.cend();
		}

		inline bool HasUbo(UboId id) const
		{
			return mUbos.find(id) != mUbos.cend();
		}

		RenderConfigs mRenderConfigs;

		std::map<UboId, std::unique_ptr<IUbo>> mUbos;
		std::vector<Mesh*> mMeshes;
	};

} // namespace erm
