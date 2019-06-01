#pragma once

#include "math/vec.h"

namespace erm {
	
	class RenderContext
	{
	public:
		RenderContext();
		
		void Draw(int count) const;
		
		void Clear() const;
		
		bool IsDepthEnabled() const;
		int GetDepthFunction() const;
		void SetDepthEnabled(bool enabled) const;
		void SetDepthFunction(int depthFunc) const;
		
		bool IsBlendEnabled() const;
		int GetBlendSourceFactor() const;
		int GetBlendDestinationFactor() const;
		void SetBlendEnabled(bool enabled) const;
		void SetBlendFunction(int sFactor, int dFactor) const;
		
		bool IsCullFaceEnabled() const;
		int GetCullFace() const;
		int GetCullFrontFace() const;
		void SetCullFaceEnabled(bool enabled) const;
		void SetCullFace(int cullFace) const;
		void SetFrontFace(int frontFace) const;
		
		int GetPolygonMode() const;
		void SetPolygonMode(int mode) const;
		
		math::vec4 GetClearColor() const;
		void SetClearColor(const math::vec4& clearColor) const;
		
	};
	
}
