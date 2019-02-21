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

	class VertexArray;
	class IndexBuffer;
	class ShaderProgram;
	
	class Renderer
	{
	public:
		Renderer(int width, int height);
		
		void Clear() const;
		void Draw(const VertexArray& va, const IndexBuffer& ib, ShaderProgram& shader, const glm::mat4& model) const;
		
	private:
		glm::mat4 mProjection;
		glm::mat4 mView;
		
	};
	
}
