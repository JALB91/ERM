#pragma once

#include <string>
#include <vector>

namespace erm {

	class FileLocator
	{
	public:
		FileLocator();

		void Refresh();

		inline const std::vector<std::string>& GetModels() const { return mModels; }
		inline const std::vector<std::string>& GetTextures() const { return mTextures; }
		inline const std::vector<std::string>& GetMaterials() const { return mMaterials; }
		inline const std::vector<std::string>& GetShaderPrograms() const { return mShaderPrograms; }
		std::vector<std::string> GetResourcesWithExtension(const char* extension, bool includeExtension = true) const;

	private:
		std::string mResourcesRoot;
		std::vector<std::string> mModels;
		std::vector<std::string> mTextures;
		std::vector<std::string> mMaterials;
		std::vector<std::string> mShaderPrograms;
	};

} // namespace erm
