#pragma once

#include <glm/glm.hpp>

#include <assert.h>

class GLFWwindow;

#define ASSERT(x) assert(x)
#define GLCALL(x) \
	GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);


namespace erm {

	class Mesh;
	class Model;
	class VertexArray;
	class IndexBuffer;
	class ShaderProgram;
	
	class Renderer
	{
	public:
		Renderer(int width, int height);
		
		void OnSizeChanged(int width, int height);
		
		void Clear() const;
		void Draw(const VertexArray& va, const IndexBuffer& ib, ShaderProgram& shader, const glm::mat4& model) const;
		void Draw(const Mesh& mesh, ShaderProgram& shader, const glm::mat4& parent = glm::mat4(1.0f)) const;
		void Draw(const Model& model, ShaderProgram& shader) const;
		
	private:
		glm::mat4 mProjection;
		glm::mat4 mView;
		
	};
	
}
