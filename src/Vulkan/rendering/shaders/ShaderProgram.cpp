#include "erm/rendering/shaders/ShaderProgram.h"

#include "erm/utils/Utils.h"

#include <vulkan/vulkan.h>

#include <fstream>
#include <iostream>

namespace {

	const std::unordered_map<erm::Uniform, const char* const> kUniforms {
		{erm::Uniform::MVP, "u_MVP"},
		{erm::Uniform::NORMAL, "u_Normal"},
		{erm::Uniform::TEXTURE_2D, "u_Texture"},
		{erm::Uniform::MODEL, "u_Model"},
		{erm::Uniform::VIEW, "u_View"},
		{erm::Uniform::PROJECTION, "u_Projection"},
		{erm::Uniform::VIEW_PROJECTION, "u_ViewProjection"},
		{erm::Uniform::MATERIAL_AMBIENT, "u_Material.ambient"},
		{erm::Uniform::MATERIAL_DIFFUSE, "u_Material.diffuse"},
		{erm::Uniform::MATERIAL_SPECULAR, "u_Material.specular"},
		{erm::Uniform::MATERIAL_SHININESS, "u_Material.shininess"},
		{erm::Uniform::LIGHT_POSITION, "u_Light.position"},
		{erm::Uniform::LIGHT_AMBIENT, "u_Light.ambient"},
		{erm::Uniform::LIGHT_DIFFUSE, "u_Light.diffuse"},
		{erm::Uniform::LIGHT_SPECULAR, "u_Light.specular"},
		{erm::Uniform::VIEW_POS, "u_ViewPos"},
		{erm::Uniform::BONE_TRANSFORM_I, "u_BoneTransforms"}};

}

namespace erm {

	unsigned int ShaderProgram::sShaderId = 0;

	ShaderProgram::ShaderProgram(const std::string& vertexShader, const std::string& fragmentShader)
		: mRendererId(CreateShaderProgram(vertexShader, fragmentShader))
		, mPath("TEMP_" + std::to_string(sShaderId))
		, mVertex(vertexShader)
		, mFragment(fragmentShader)
		, mId(sShaderId++)
	{
		CacheUniformsLocations();
	}

	ShaderProgram::ShaderProgram(const std::string& /*fragmentShader*/)
		: mId(0)
	{
		CacheUniformsLocations();
	}

	ShaderProgram::~ShaderProgram()
	{
		//GL_CALL(glDeleteProgram(mRendererId));
	}

	void ShaderProgram::Bind() const
	{
		//GL_CALL(glUseProgram(mRendererId));
	}

	void ShaderProgram::Unbind() const
	{
		//GL_CALL(glUseProgram(0));
	}

	void ShaderProgram::SetShaderSources(const std::string& vertexShader, const std::string& fragmentShader)
	{
		//GL_CALL(glDeleteProgram(mRendererId));
		mRendererId = CreateShaderProgram(vertexShader, fragmentShader);
		mVertex = vertexShader;
		mFragment = fragmentShader;
		mUniformLocationsCache.clear();
		CacheUniformsLocations();
	}

	void ShaderProgram::SetUniform1i(const Uniform& uniform, int /*value*/, int index /*= -1*/)
	{
		std::string uniformName = kUniforms.at(uniform);

		if (index >= 0)
		{
			uniformName += "[" + std::to_string(index) + "]";
		}
	}

	void ShaderProgram::SetUniform1f(const Uniform& uniform, float /*value*/, int index /*= -1*/)
	{
		std::string uniformName = kUniforms.at(uniform);

		if (index >= 0)
		{
			uniformName += "[" + std::to_string(index) + "]";
		}
	}

	void ShaderProgram::SetUniform3f(const Uniform& uniform, math::vec3 /*value*/, int index /*= -1*/)
	{
		std::string uniformName = kUniforms.at(uniform);

		if (index >= 0)
		{
			uniformName += "[" + std::to_string(index) + "]";
		}
	}

	void ShaderProgram::SetUniform3f(const Uniform& uniform, float v0, float v1, float v2, int index /*= -1*/)
	{
		UNUSED(v0);
		UNUSED(v1);
		UNUSED(v2);

		std::string uniformName = kUniforms.at(uniform);

		if (index >= 0)
		{
			uniformName += "[" + std::to_string(index) + "]";
		}
	}

	void ShaderProgram::SetUniform4f(const Uniform& uniform, math::vec4 value, int index /*= -1*/)
	{
		UNUSED(value);

		std::string uniformName = kUniforms.at(uniform);

		if (index >= 0)
		{
			uniformName += "[" + std::to_string(index) + "]";
		}
	}

	void ShaderProgram::SetUniform4f(const Uniform& uniform, float v0, float v1, float v2, float v3, int index /*= -1*/)
	{
		UNUSED(v0);
		UNUSED(v1);
		UNUSED(v2);
		UNUSED(v3);

		std::string uniformName = kUniforms.at(uniform);

		if (index >= 0)
		{
			uniformName += "[" + std::to_string(index) + "]";
		}
	}

	void ShaderProgram::SetUniformMat4f(const Uniform& uniform, const math::mat4& matrix, int index /*= -1*/)
	{
		UNUSED(matrix);

		std::string uniformName = kUniforms.at(uniform);

		if (index >= 0)
		{
			uniformName += "[" + std::to_string(index) + "]";
		}
	}

	std::string ShaderProgram::ParseShader(const std::string& path) const
	{
		UNUSED(path);
		return "";
	}

	unsigned int ShaderProgram::CompileShader(unsigned int type, const std::string& source) const
	{
		UNUSED(type);
		UNUSED(source);
		return 0;
	}

	unsigned int ShaderProgram::CreateShaderProgram(const std::string& vertex, const std::string& fragment) const
	{
		UNUSED(vertex);
		UNUSED(fragment);
		return 0;
	}

	void ShaderProgram::CacheUniformsLocations()
	{}

	int ShaderProgram::GetUniformLocation(const std::string& name)
	{
		if (mUniformLocationsCache.find(name) != mUniformLocationsCache.end())
		{
			return mUniformLocationsCache.at(name);
		}

		std::cout << "Uniform location not found" << std::endl;

		return -1;
	}

} // namespace erm
