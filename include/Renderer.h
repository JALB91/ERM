#pragma once

#include <assert.h>

#define ASSERT(x) assert(x)
#define GLCALL(x) \
	GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);
