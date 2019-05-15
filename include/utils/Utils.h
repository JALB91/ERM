#pragma once

#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <assert.h>

#define ASSERT(x) assert(x)
#define EXPECT(x, msg) erm::Utils::LogCall((x), msg, #x, __FILE__, __LINE__)
#define GLCALL(x) \
	erm::Utils::GLClearError();\
	x;\
	ASSERT(erm::Utils::GLLogCall(#x, __FILE__, __LINE__))

namespace erm {
	
	namespace Utils {
	
		bool LogCall(bool cond, const char* msg, const char* function, const char* file, int line);
		
		bool GLLogCall(const char* function, const char* file, int line);
		void GLClearError();
		
		std::vector<std::string> SplitString(const std::string& str, char ch);
		std::string GetRelativePath(const char* absolutePath);
		
		float MagnitudeV3(const glm::vec3 in);
		float DotV3(const glm::vec3 a, const glm::vec3 b);
		float AngleBetweenV3(const glm::vec3 a, const glm::vec3 b);
		glm::vec3 ProjV3(const glm::vec3 a, const glm::vec3 b);
		glm::vec3 GenTriNormal(glm::vec3 t1, glm::vec3 t2, glm::vec3 t3);
		bool IsSameSide(glm::vec3 p1, glm::vec3 p2, glm::vec3 a, glm::vec3 b);
		bool IsInTriangle(glm::vec3 point, glm::vec3 tri1, glm::vec3 tri2, glm::vec3 tri3);
		
	}
	
}
