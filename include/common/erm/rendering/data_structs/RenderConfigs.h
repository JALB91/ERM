#pragma once

#include "erm/math/BoundingBox.h"

#include "erm/rendering/enums/BlendFunction.h"
#include "erm/rendering/enums/CullMode.h"
#include "erm/rendering/enums/DepthFunction.h"
#include "erm/rendering/enums/DrawMode.h"
#include "erm/rendering/enums/FrontFace.h"
#include "erm/rendering/enums/PolygonMode.h"

#include <string>

namespace erm {
	class ShaderProgram;
	class Texture;
	struct Material;
} // namespace erm

namespace erm {

	struct RenderConfigs
	{
		RenderConfigs();

		bool operator==(const RenderConfigs& other) const;
		bool operator!=(const RenderConfigs& other) const;
		bool operator<(const RenderConfigs& other) const;

		bool IsBlendCompatible(const RenderConfigs& other) const;
		bool IsDepthCompatible(const RenderConfigs& other) const;
		bool IsCullCompatible(const RenderConfigs& other) const;
		bool IsDrawModeCompatible(const RenderConfigs& other) const;
		bool IsPolyModeCompatible(const RenderConfigs& other) const;
		bool IsShaderCompatible(const RenderConfigs& other) const;
		bool IsMaterialCompatible(const RenderConfigs& other) const;
		bool IsTextureCompatible(const RenderConfigs& other) const;

		BoundingBox2D mViewport;
		bool mDepthTestEnabled;
		DepthFunction mDepthFunction;
		bool mBlendEnabled;
		BlendFunction mBlendFunction;
		bool mCullEnabled;
		CullMode mCullMode;
		FrontFace mFrontFace;
		PolygonMode mPolygonMode;
		DrawMode mDrawMode;
		ShaderProgram* mShaderProgram;
		Material* mMaterial;
		Texture* mTexture;
	};

} // namespace erm
