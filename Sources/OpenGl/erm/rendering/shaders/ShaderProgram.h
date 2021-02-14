#pragma once

#include "erm/math/mat.h"
#include "erm/math/vec.h"

#include "erm/rendering/shaders/Uniform.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace erm {
	class Device;
}

namespace erm {

	class ShaderProgram
	{
	public:
		ShaderProgram(Device& device, const std::string& vertexShader, const std::string& fragmentShader);
		ShaderProgram(Device& device, const std::string& shaderPath);
		~ShaderProgram();

		ShaderProgram(ShaderProgram&&) = delete;
		ShaderProgram(const ShaderProgram&) = delete;

		ShaderProgram& operator=(ShaderProgram&&) = delete;
		ShaderProgram& operator=(const ShaderProgram&) = delete;

		void Bind() const;
		void Unbind() const;

		const std::string& GetPath() const { return mPath; }
		const std::string& GetVertexSource() const { return mVertex; }
		const std::string& GetFragmentSource() const { return mFragment; }

		void SetShaderSources(const std::string& vertexShader, const std::string& fragmentShader);

		void SetUniform1i(const Uniform& uniform, int value, int index = -1);
		void SetUniform1f(const Uniform& uniform, float value, int index = -1);

		void SetUniform3f(const Uniform& uniform, math::vec3 value, int index = -1);
		void SetUniform3f(const Uniform& uniform, float v0, float v1, float v2, int index = -1);

		void SetUniform4f(const Uniform& uniform, math::vec4 value, int index = -1);
		void SetUniform4f(const Uniform& uniform, float v0, float v1, float v2, float v3, int index = -1);

		void SetUniformMat4f(const Uniform& uniform, const math::mat4& matrix, int index = -1);

	private:
		std::string ParseShader(const std::string& path) const;
		unsigned int CompileShader(unsigned int type, const std::string& source) const;
		unsigned int CreateShaderProgram(const std::string& vertexSource, const std::string& fragmentSource) const;

		void CacheUniformsLocations();
		int GetUniformLocation(const std::string& name);

		std::unordered_map<std::string, int> mUniformLocationsCache;
		unsigned int mRendererId;
		std::string mPath;
		std::string mVertex;
		std::string mFragment;
		const unsigned int mId;

	private:
		static unsigned int sShaderId;
	};

} // namespace erm
