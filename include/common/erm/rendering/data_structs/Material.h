#pragma once

#include "erm/math/vec.h"

#include "erm/rendering/data_structs/IAsset.h"

namespace erm {
	class Texture;
}

namespace erm {

	struct Material : public IAsset
	{
		static Material DEFAULT;

		Material(
			const char* path,
			const char* name,
			const math::vec3& ambient,
			const math::vec3& diffuse,
			const math::vec3& specular,
			float shininess,
			Texture* diffuseMap,
			Texture* normalMap,
			Texture* specularMap)
			: IAsset(path, name)
			, mAmbient(ambient)
			, mDiffuse(diffuse)
			, mSpecular(specular)
			, mShininess(shininess)
			, mDiffuseMap(diffuseMap)
			, mNormalMap(normalMap)
			, mSpecularMap(specularMap)
		{}

		math::vec3 mAmbient;
		math::vec3 mDiffuse;
		math::vec3 mSpecular;
		float mShininess;
		Texture* mDiffuseMap;
		Texture* mNormalMap;
		Texture* mSpecularMap;
	};

} // namespace erm
