#pragma once

#include "erm/rendering/data_structs/IAsset.h"
#include "erm/rendering/enums/ShaderType.h"
#include "erm/rendering/shaders/ShaderData.h"

#include <unordered_map>

namespace erm {
	class Device;
}

namespace erm {

	class ShaderProgram : public IAsset
	{
	public:
		ShaderProgram(Device& device, const char* path);

		inline const std::unordered_map<erm::ShaderType, std::vector<erm::ShaderData>>& GetShadersDataMap() { return mShadersDataMap; }

	private:
		Device& mDevice;

		std::unordered_map<erm::ShaderType, std::vector<erm::ShaderData>> mShadersDataMap;
	};

} // namespace erm