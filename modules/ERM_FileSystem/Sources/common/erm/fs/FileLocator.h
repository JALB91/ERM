#pragma once

#include "erm/fs/fs.h"

#include <erm/modules_lib/IModuleObject.h>

#include <refl.hpp>

#include <string>
#include <string_view>
#include <vector>

namespace erm {

class FileLocator : public IModuleObject<FileLocator>
{
public:
	FileLocator() noexcept;
	
	bool isERMRoot(const fs::path& path) const;
	
	bool setERMRootPath(fs::path ermRootPath);
	inline const fs::path& getERMRootPath() const { return mERMRootPath; }

	inline const std::vector<std::string>& getSounds() const { return mSounds; }
	inline const std::vector<std::string>& getModels() const { return mModels; }
	inline const std::vector<std::string>& getTextures() const { return mTextures; }
	inline const std::vector<std::string>& getMaterials() const { return mMaterials; }
	inline const std::vector<std::string>& getShaderPrograms() const { return mShaderPrograms; }

private:
	void refresh();
	std::vector<std::string> getResourcesWithExtension(const char* extension, bool includeExtension = true) const;

	fs::path mERMRootPath;
	std::vector<std::string> mSounds;
	std::vector<std::string> mModels;
	std::vector<std::string> mTextures;
	std::vector<std::string> mMaterials;
	std::vector<std::string> mShaderPrograms;
};

} // namespace erm

REFL_AUTO(type(erm::FileLocator));
