#pragma once

#include "erm/rendering/enums/DrawMode.h"
#include "erm/rendering/enums/PolygonMode.h"
#include "erm/rendering/enums/DepthFunction.h"
#include "erm/rendering/enums/BlendFunction.h"
#include "erm/rendering/enums/CullFace.h"
#include "erm/rendering/enums/FrontFace.h"

#include "erm/math/vec.h"

namespace erm {
	
	class RenderContext
	{
	public:
		RenderContext();
		
		void Draw(DrawMode drawMode, unsigned int count) const;
		
		void Clear() const;
		
		bool IsDepthEnabled() const;
		DepthFunction GetDepthFunction() const;
		void SetDepthEnabled(bool enabled) const;
		void SetDepthFunction(DepthFunction depthFunc) const;
		
		bool IsBlendEnabled() const;
		BlendFunction GetBlendSourceFactor() const;
		BlendFunction GetBlendDestinationFactor() const;
		void SetBlendEnabled(bool enabled) const;
		void SetBlendFunction(BlendFunction sFactor, BlendFunction dFactor) const;
		
		bool IsCullFaceEnabled() const;
		CullFace GetCullFace() const;
		FrontFace GetFrontFace() const;
		void SetCullFaceEnabled(bool enabled) const;
		void SetCullFace(CullFace cullFace) const;
		void SetFrontFace(FrontFace frontFace) const;
		
		PolygonMode GetPolygonMode() const;
		void SetPolygonMode(PolygonMode mode) const;
		
		math::vec4 GetClearColor() const;
		void SetClearColor(const math::vec4& clearColor) const;
		
	};
	
}
