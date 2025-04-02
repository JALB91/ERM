#pragma once

#include <string>
#include <vector>

namespace erm {

class FileLocator
{
public:
	FileLocator();

	inline const std::vector<std::string>& getSounds() const { return mSounds; }
	inline const std::vector<std::string>& getModels() const { return mModels; }
	inline const std::vector<std::string>& getTextures() const { return mTextures; }
	inline const std::vector<std::string>& getMaterials() const { return mMaterials; }
	inline const std::vector<std::string>& getShaderPrograms() const { return mShaderPrograms; }

private:
	void refresh();
	std::vector<std::string> getResourcesWithExtension(const char* extension, bool includeExtension = true) const;

	std::vector<std::string> mSounds;
	std::vector<std::string> mModels;
	std::vector<std::string> mTextures;
	std::vector<std::string> mMaterials;
	std::vector<std::string> mShaderPrograms;
};

} // namespace erm
