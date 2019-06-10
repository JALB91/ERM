#include "rendering/RenderContext.h"
#include "utils/Utils.h"

#include <GL/glew.h>

#include <iostream>

namespace erm {
	
	RenderContext::RenderContext()
	{
		std::cout << glGetString(GL_VERSION) << std::endl;
		std::cout << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	}
	
	void RenderContext::Draw(int count) const
	{
		GLCALL(glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr));
	}
	
	void RenderContext::Clear() const
	{
		GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	}
	
	bool RenderContext::IsDepthEnabled() const
	{
		GLCALL(return glIsEnabled(GL_DEPTH_TEST));
	}
	
	int RenderContext::GetDepthFunction() const
	{
		int result;
		GLCALL(glGetIntegerv(GL_DEPTH_FUNC, &result));
		return result;
	}
	
	void RenderContext::SetDepthEnabled(bool enabled) const
	{
		GLCALL(enabled ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST));
	}
	
	void RenderContext::SetDepthFunction(int depthFunc) const
	{
		GLCALL(glDepthFunc(depthFunc));
	}
	
	bool RenderContext::IsBlendEnabled() const
	{
		GLCALL(return glIsEnabled(GL_BLEND));
	}
	
	int RenderContext::GetBlendSourceFactor() const
	{
		int result;
		GLCALL(glGetIntegerv(GL_BLEND_SRC, &result));
		return result;
	}
	
	int RenderContext::GetBlendDestinationFactor() const
	{
		int result;
		GLCALL(glGetIntegerv(GL_BLEND_DST, &result));
		return result;
	}
	
	void RenderContext::SetBlendEnabled(bool enabled) const
	{
		GLCALL(enabled ? glEnable(GL_BLEND) : glDisable(GL_BLEND));
	}
	
	void RenderContext::SetBlendFunction(int sFactor, int dFactor) const
	{
		GLCALL(glBlendFunc(sFactor, dFactor));
	}
	
	bool RenderContext::IsCullFaceEnabled() const
	{
		GLCALL(return glIsEnabled(GL_CULL_FACE));
	}
	
	int RenderContext::GetCullFace() const
	{
		int result;
		GLCALL(glGetIntegerv(GL_CULL_FACE_MODE, &result));
		return result;
	}
	
	int RenderContext::GetCullFrontFace() const
	{
		int result;
		GLCALL(glGetIntegerv(GL_FRONT_FACE, &result));
		return result;
	}
	
	void RenderContext::SetCullFaceEnabled(bool enabled) const
	{
		GLCALL(enabled ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE));
	}
	
	void RenderContext::SetCullFace(int cullFace) const
	{
		GLCALL(glCullFace(cullFace));
	}
	
	void RenderContext::SetFrontFace(int frontFace) const
	{
		GLCALL(glFrontFace(frontFace));
	}
	
	int RenderContext::GetPolygonMode() const
	{
		int result;
		GLCALL(glGetIntegerv(GL_POLYGON_MODE, &result));
		return result;
	}
	
	void RenderContext::SetPolygonMode(int mode) const
	{
		GLCALL(glPolygonMode(GL_FRONT_AND_BACK, mode));
	}
	
	math::vec4 RenderContext::GetClearColor() const
	{
		math::vec4 result;
		GLCALL(glGetFloatv(GL_COLOR_CLEAR_VALUE, &result.x));
		return result;
	}
	
	void RenderContext::SetClearColor(const math::vec4& clearColor) const
	{
		GLCALL(glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w));
	}
	
}
