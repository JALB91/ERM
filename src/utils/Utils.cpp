#include "utils/Utils.h"

#include <GL/glew.h>

#include <iostream>
#include <math.h>

namespace erm {
	
	namespace Utils {
		
		bool LogCall(bool cond, const char* msg, const char* function, const char* file, int line)
		{
			if (!cond)
			{
				std::cout << "[Assert] (" << msg << ") " << function << " " << file << ":" << line << std::endl;
				ASSERT(cond);
			}
			return cond;
		}
		
		bool GLLogCall(const char* function, const char* file, int line)
		{
			while (GLenum error = glGetError())
			{
				std::cout << "[OpenGL error] (" << error << ") " << function << " " << file << ":" << line << std::endl;
				return false;
			}
			return true;
		}
		
		void GLClearError()
		{
			while (glGetError() != GL_NO_ERROR);
		}
		
		std::vector<std::string> SplitString(const std::string& str, char ch)
		{
			std::vector<std::string> vec;
			std::string res = "";
			
			for (auto iter = str.cbegin(); iter != str.cend(); ++iter)
			{
				if (*iter != ch)
				{
					res += *iter;
				}
				else
				{
					vec.push_back(res);
					res = "";
				}
			}
			
			vec.push_back(res);
			
			return vec;
		}
		
		std::string GetRelativePath(const char* absolutePath)
		{
#if defined(WIN32)
			return std::string("../") + absolutePath;
#elif defined(__APPLE__)
			return absolutePath;
#else
			return absolutePath;
#endif
		}
		
		glm::vec3 CrossV3(const glm::vec3 a, const glm::vec3 b)
		{
			return glm::vec3(
				a.y * b.z - a.z * b.y,
				a.z * b.x - a.x * b.z,
				a.x * b.y - a.y * b.x
			);
		}
		
		float MagnitudeV3(const glm::vec3 in)
		{
			return (sqrtf(powf(in.x, 2) + powf(in.y, 2) + powf(in.z, 2)));
		}
		
		float DotV3(const glm::vec3 a, const glm::vec3 b)
		{
			return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
		}
		
		float AngleBetweenV3(const glm::vec3 a, const glm::vec3 b)
		{
			float angle = DotV3(a, b);
			angle /= (MagnitudeV3(a) * MagnitudeV3(b));
			angle = acosf(angle);
			return angle;
		}
		
		glm::vec3 ProjV3(const glm::vec3 a, const glm::vec3 b)
		{
			const glm::vec3 bn = b / MagnitudeV3(b);
			return bn * DotV3(a, bn);
		}
		
		glm::vec3 GenTriNormal(glm::vec3 t1, glm::vec3 t2, glm::vec3 t3)
		{
			const glm::vec3 u = t2 - t1;
			const glm::vec3 v = t3 - t1;
			
			return CrossV3(u, v);
		}
		
		bool IsSameSide(glm::vec3 p1, glm::vec3 p2, glm::vec3 a, glm::vec3 b)
		{
			const glm::vec3 cp1 = CrossV3(b - a, p1 - a);
			const glm::vec3 cp2 = CrossV3(b - a, p2 - a);
			
			return (DotV3(cp1, cp2) >= 0);
		}
		
		bool IsInTriangle(glm::vec3 point, glm::vec3 tri1, glm::vec3 tri2, glm::vec3 tri3)
		{
			bool within_tri_prisim(
				IsSameSide(point, tri1, tri2, tri3) &&
				IsSameSide(point, tri2, tri1, tri3) &&
				IsSameSide(point, tri3, tri1, tri2)
			);
			
			if (!within_tri_prisim)
			{
				return false;
			}
			
			const glm::vec3 n = GenTriNormal(tri1, tri2, tri3);
			const glm::vec3 proj = ProjV3(point, n);
			
			return (MagnitudeV3(proj) == 0);
		}
		
	}
	
}
