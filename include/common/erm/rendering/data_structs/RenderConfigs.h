#pragma once

#include "erm/math/BoundingBox.h"

#include "erm/rendering/data_structs/SubpassData.h"

#include "erm/rendering/enums/BlendFunction.h"
#include "erm/rendering/enums/CullMode.h"
#include "erm/rendering/enums/DepthFunction.h"
#include "erm/rendering/enums/DrawMode.h"
#include "erm/rendering/enums/FrontFace.h"
#include "erm/rendering/enums/PolygonMode.h"

#include "erm/utils/Utils.h"

namespace erm {
	class ShaderProgram;
	class Texture;
	struct Material;
} // namespace erm

namespace erm {

	struct RenderConfigs
	{
		static const RenderConfigs MODELS_RENDER_CONFIGS;

		RenderConfigs(
			const SubpassData& subpassData,
			std::optional<BoundingBox2D> normViewport = {},
			std::optional<bool> depthTestEnabled = {},
			std::optional<bool> depthWriteEnabled = {},
			std::optional<DepthFunction> depthFunction = {},
			std::optional<bool> blendEnabled = {},
			std::optional<BlendFunction> blendFunction = {},
			std::optional<CullMode> cullMode = {},
			std::optional<FrontFace> frontFace = {},
			std::optional<PolygonMode> polygonMode = {},
			std::optional<DrawMode> drawMode = {},
			ShaderProgram* shaderProgram = nullptr,
			Material* material = nullptr,
			Texture* texture = nullptr);

		bool operator==(const RenderConfigs& other) const;
		bool operator!=(const RenderConfigs& other) const;

		bool IsRenderPassLevelCompatible(const RenderConfigs& other) const;
		bool IsPipelineLevelCompatible(const RenderConfigs& other) const;
		bool IsResourcesBindingCompatible(const RenderConfigs& other) const;

		bool IsSubpassCompatible(const RenderConfigs& other) const;
		bool IsBlendCompatible(const RenderConfigs& other) const;
		bool IsDepthCompatible(const RenderConfigs& other) const;
		bool IsCullCompatible(const RenderConfigs& other) const;
		bool IsDrawModeCompatible(const RenderConfigs& other) const;
		bool IsPolyModeCompatible(const RenderConfigs& other) const;
		bool IsShaderCompatible(const RenderConfigs& other) const;
		bool IsMaterialCompatible(const RenderConfigs& other) const;
		bool IsTextureCompatible(const RenderConfigs& other) const;

		SubpassData mSubpassData;
		DECL_SET_GET_OPT_WITH_DEFAULT(NormViewport, BoundingBox2D, BoundingBox2D({0.0f, 0.0f}, {1.0f, 1.0f}));
		DECL_SET_GET_OPT_WITH_DEFAULT(DepthTestEnabled, bool, true);
		DECL_SET_GET_OPT_WITH_DEFAULT(DepthWriteEnabled, bool, true);
		DECL_SET_GET_OPT_WITH_DEFAULT(DepthFunction, DepthFunction, DepthFunction::LESS);
		DECL_SET_GET_OPT_WITH_DEFAULT(BlendEnabled, bool, false);
		DECL_SET_GET_OPT_WITH_DEFAULT(BlendFunction, BlendFunction, BlendFunction::ONE_MINUS_SRC_ALPHA);
		DECL_SET_GET_OPT_WITH_DEFAULT(CullMode, CullMode, CullMode::BACK);
		DECL_SET_GET_OPT_WITH_DEFAULT(FrontFace, FrontFace, FrontFace::CCW);
		DECL_SET_GET_OPT_WITH_DEFAULT(PolygonMode, PolygonMode, PolygonMode::FILL);
		DECL_SET_GET_OPT_WITH_DEFAULT(DrawMode, DrawMode, DrawMode::TRIANGLES);
		ShaderProgram* mShaderProgram;
		Material* mMaterial;
		Texture* mTexture;
	};

} // namespace erm
