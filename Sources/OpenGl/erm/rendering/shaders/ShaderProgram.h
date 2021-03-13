#pragma once

#include "erm/rendering/shaders/IShaderProgram.h"
#include "erm/rendering/shaders/Uniform.h"

namespace erm {

	class ShaderProgram : public IShaderProgram
	{
	public:
		ShaderProgram(Device& device, const char* shaderPath);
		~ShaderProgram();

		void Bind() const;
		void Unbind() const;

		void SetUniform1i(const Uniform& uniform, int value, int index = -1);
		void SetUniform1f(const Uniform& uniform, float value, int index = -1);

		void SetUniform3f(const Uniform& uniform, math::vec3 value, int index = -1);
		void SetUniform3f(const Uniform& uniform, float v0, float v1, float v2, int index = -1);

		void SetUniform4f(const Uniform& uniform, math::vec4 value, int index = -1);
		void SetUniform4f(const Uniform& uniform, float v0, float v1, float v2, float v3, int index = -1);

		void SetUniformMat4f(const Uniform& uniform, const math::mat4& matrix, int index = -1);

	private:
		// IShaderProgram
		void UpdateBindingData() override;

		std::string ParseShader(const std::string& path) const;
		unsigned int CompileShader(unsigned int type, const std::string& source) const;
		unsigned int CreateShaderProgram(const std::string& vertexSource, const std::string& fragmentSource) const;

		void CacheUniformsLocations();
		int GetUniformLocation(const std::string& name);

		std::unordered_map<std::string, int> mUniformLocationsCache;
		unsigned int mRendererId;
	};

} // namespace erm
