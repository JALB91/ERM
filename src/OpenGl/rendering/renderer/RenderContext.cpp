#include "erm/rendering/renderer/RenderContext.h"

#include "erm/utils/GlMacros.h"
#include "erm/utils/GlUtils.h"

#include <GL/glew.h>

#include <iostream>

namespace erm {
	
	RenderContext::RenderContext()
	{
		std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;
		std::cout << "GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	}
	
	void RenderContext::Draw(DrawMode drawMode, unsigned int count) const
	{
		GL_CALL(glDrawElements(DrawModeToInt(drawMode), count, GL_UNSIGNED_INT, nullptr));
	}
	
	void RenderContext::Clear() const
	{
		GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	}
	
	bool RenderContext::IsDepthEnabled() const
	{
		GL_CALL(const bool result = glIsEnabled(GL_DEPTH_TEST));
		return result;
	}
	
	DepthFunction RenderContext::GetDepthFunction() const
	{
		int result;
		GL_CALL(glGetIntegerv(GL_DEPTH_FUNC, &result));
		return IntToDepthFunction(result);
	}
	
	void RenderContext::SetDepthEnabled(bool enabled) const
	{
		GL_CALL(enabled ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST));
	}
	
	void RenderContext::SetDepthFunction(DepthFunction depthFunc) const
	{
		GL_CALL(glDepthFunc(DepthFunctionToInt(depthFunc)));
	}
	
	bool RenderContext::IsBlendEnabled() const
	{
		GL_CALL(const bool result = glIsEnabled(GL_BLEND));
		return result;
	}
	
	BlendFunction RenderContext::GetBlendSourceFactor() const
	{
		int result;
		GL_CALL(glGetIntegerv(GL_BLEND_SRC, &result));
		return IntToBlendFunction(result);
	}
	
	BlendFunction RenderContext::GetBlendDestinationFactor() const
	{
		int result;
		GL_CALL(glGetIntegerv(GL_BLEND_DST, &result));
		return IntToBlendFunction(result);
	}
	
	void RenderContext::SetBlendEnabled(bool enabled) const
	{
		GL_CALL(enabled ? glEnable(GL_BLEND) : glDisable(GL_BLEND));
	}
	
	void RenderContext::SetBlendFunction(BlendFunction sFactor, BlendFunction dFactor) const
	{
		GL_CALL(glBlendFunc(BlendFunctionToInt(sFactor), BlendFunctionToInt(dFactor)));
	}
	
	bool RenderContext::IsCullFaceEnabled() const
	{
		GL_CALL(const bool result = glIsEnabled(GL_CULL_FACE));
		return result;
	}
	
	CullFace RenderContext::GetCullFace() const
	{
		int result;
		GL_CALL(glGetIntegerv(GL_CULL_FACE_MODE, &result));
		return IntToCullFace(result);
	}
	
	FrontFace RenderContext::GetFrontFace() const
	{
		int result;
		GL_CALL(glGetIntegerv(GL_FRONT_FACE, &result));
		return IntToFrontFace(result);
	}
	
	void RenderContext::SetCullFaceEnabled(bool enabled) const
	{
		GL_CALL(enabled ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE));
	}
	
	void RenderContext::SetCullFace(CullFace cullFace) const
	{
		GL_CALL(glCullFace(CullFaceToInt(cullFace)));
	}
	
	void RenderContext::SetFrontFace(FrontFace frontFace) const
	{
		GL_CALL(glFrontFace(FrontFaceToInt(frontFace)));
	}
	
	PolygonMode RenderContext::GetPolygonMode() const
	{
		int result;
		GL_CALL(glGetIntegerv(GL_POLYGON_MODE, &result));
		return IntToPolygonMode(result);
	}
	
	void RenderContext::SetPolygonMode(PolygonMode mode) const
	{
		GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, PolygonModeToInt(mode)));
	}
	
	math::vec4 RenderContext::GetClearColor() const
	{
		math::vec4 result;
		GL_CALL(glGetFloatv(GL_COLOR_CLEAR_VALUE, &result.x));
		return result;
	}
	
	void RenderContext::SetClearColor(const math::vec4& clearColor) const
	{
		GL_CALL(glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w));
	}
	
}
