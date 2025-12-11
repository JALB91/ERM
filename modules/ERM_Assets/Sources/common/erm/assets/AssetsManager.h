#pragma once

#include "erm/assets/AssetHandle.h"
#include "erm/assets/shaders/ShaderProgram.h"
#include "erm/assets/textures/Texture.h"

#include <erm/fs/fs.h>
#include <erm/modules_lib/IModuleObject.h>
#include <erm/utils/StringID.h>

#include <refl.hpp>

namespace erm {
class AssetsRepo;
struct ShaderProgram;
struct Texture;
}

namespace erm {

class AssetsManager : public IModuleObject<AssetsManager>
{
public:
	AssetsManager() noexcept;

	AssetHandle<Texture> loadTexture(const fs::path& path);
	AssetHandle<ShaderProgram> loadShaderProgram(const fs::path& path);

	template<typename T>
	bool isStillLoading(AssetHandle<T> /*assetID*/) const
	{
		return false;
	}

private:
	AssetsRepo& mAssetsRepo;

};

}

REFL_AUTO(type(erm::AssetsManager));
