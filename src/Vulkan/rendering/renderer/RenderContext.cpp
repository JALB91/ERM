#include "erm/rendering/renderer/RenderContext.h"
#include "erm/utils/Utils.h"

#include <iostream>

namespace erm {
	
	RenderContext::RenderContext()
	{}
	
	void RenderContext::Draw(DrawMode /*drawMode*/, unsigned int /*count*/) const
	{}
	
	void RenderContext::Clear() const
	{}
	
	bool RenderContext::IsDepthEnabled() const
	{
		return false;
	}
	
	DepthFunction RenderContext::GetDepthFunction() const
	{
		return DepthFunction::ALWAYS;
	}
	
	void RenderContext::SetDepthEnabled(bool /*enabled*/) const
	{}
	
	void RenderContext::SetDepthFunction(DepthFunction /*depthFunc*/) const
	{}
	
	bool RenderContext::IsBlendEnabled() const
	{
		return false;
	}
	
	BlendFunction RenderContext::GetBlendSourceFactor() const
	{
		return BlendFunction::CONSTANT_ALPHA;
	}
	
	BlendFunction RenderContext::GetBlendDestinationFactor() const
	{
		return BlendFunction::CONSTANT_ALPHA;
	}
	
	void RenderContext::SetBlendEnabled(bool /*enabled*/) const
	{}
	
	void RenderContext::SetBlendFunction(BlendFunction /*sFactor*/, BlendFunction /*dFactor*/) const
	{}
	
	bool RenderContext::IsCullFaceEnabled() const
	{
		return false;
	}
	
	CullFace RenderContext::GetCullFace() const
	{
		return CullFace::BACK;
	}
	
	FrontFace RenderContext::GetFrontFace() const
	{
		return FrontFace::CCW;
	}
	
	void RenderContext::SetCullFaceEnabled(bool /*enabled*/) const
	{}
	
	void RenderContext::SetCullFace(CullFace /*cullFace*/) const
	{}
	
	void RenderContext::SetFrontFace(FrontFace /*frontFace*/) const
	{}
	
	PolygonMode RenderContext::GetPolygonMode() const
	{
		return PolygonMode::FILL;
	}
	
	void RenderContext::SetPolygonMode(PolygonMode /*mode*/) const
	{}
	
	math::vec4 RenderContext::GetClearColor() const
	{
		return {};
	}
	
	void RenderContext::SetClearColor(const math::vec4& /*clearColor*/) const
	{}
	
}
