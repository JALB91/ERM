#include "erm/utils/GlUtils.h"

#include "erm/utils/Utils.h"

#include <GL/glew.h>

namespace erm {

	int DepthFunctionToInt(DepthFunction function)
	{
		switch (function)
		{
			case DepthFunction::ALWAYS:
				return GL_ALWAYS;
			case DepthFunction::EQUAL:
				return GL_EQUAL;
			case DepthFunction::GEQUAL:
				return GL_GEQUAL;
			case DepthFunction::GREATER:
				return GL_GREATER;
			case DepthFunction::LEQUAL:
				return GL_LEQUAL;
			case DepthFunction::LESS:
				return GL_LESS;
			case DepthFunction::NEVER:
				return GL_NEVER;
			case DepthFunction::NOT_EQUAL:
				return GL_NOTEQUAL;
			default:
				ASSERT(false);
				return GL_ALWAYS;
		}
	}

	DepthFunction IntToDepthFunction(int function)
	{
		switch (function)
		{
			case GL_ALWAYS:
				return DepthFunction::ALWAYS;
			case GL_NEVER:
				return DepthFunction::NEVER;
			case GL_LESS:
				return DepthFunction::LESS;
			case GL_EQUAL:
				return DepthFunction::EQUAL;
			case GL_LEQUAL:
				return DepthFunction::LEQUAL;
			case GL_GREATER:
				return DepthFunction::GREATER;
			case GL_NOTEQUAL:
				return DepthFunction::NOT_EQUAL;
			case GL_GEQUAL:
				return DepthFunction::GEQUAL;
			default:
				ASSERT(false);
				return DepthFunction::ALWAYS;
		}
	}

	int BlendFunctionToInt(BlendFunction function)
	{
		switch (function)
		{
			case BlendFunction::CONSTANT_ALPHA:
				return GL_CONSTANT_ALPHA;
			case BlendFunction::CONSTANT_COLOR:
				return GL_CONSTANT_COLOR;
			case BlendFunction::DST_ALPHA:
				return GL_DST_ALPHA;
			case BlendFunction::DST_COLOR:
				return GL_DST_COLOR;
			case BlendFunction::ONE:
				return GL_ONE;
			case BlendFunction::ONE_MINUS_CONSTANT_ALPHA:
				return GL_ONE_MINUS_CONSTANT_ALPHA;
			case BlendFunction::ONE_MINUS_CONSTANT_COLOR:
				return GL_ONE_MINUS_CONSTANT_COLOR;
			case BlendFunction::ONE_MINUS_DST_ALPHA:
				return GL_ONE_MINUS_DST_ALPHA;
			case BlendFunction::ONE_MINUS_DST_COLOR:
				return GL_ONE_MINUS_DST_COLOR;
			case BlendFunction::ONE_MINUS_SRC_ALPHA:
				return GL_ONE_MINUS_SRC_ALPHA;
			case BlendFunction::ONE_MINUS_SRC_COLOR:
				return GL_ONE_MINUS_SRC_COLOR;
			case BlendFunction::SRC_ALPHA:
				return GL_SRC_ALPHA;
			case BlendFunction::SRC_COLOR:
				return GL_SRC_COLOR;
			default:
				ASSERT(false);
				return GL_CONSTANT_ALPHA;
		}
	}

	BlendFunction IntToBlendFunction(int function)
	{
		switch (function)
		{
			case GL_CONSTANT_ALPHA:
				return BlendFunction::CONSTANT_ALPHA;
			case GL_CONSTANT_COLOR:
				return BlendFunction::CONSTANT_COLOR;
			case GL_DST_ALPHA:
				return BlendFunction::DST_ALPHA;
			case GL_DST_COLOR:
				return BlendFunction::DST_COLOR;
			case GL_ONE:
				return BlendFunction::ONE;
			case GL_ONE_MINUS_CONSTANT_ALPHA:
				return BlendFunction::ONE_MINUS_CONSTANT_ALPHA;
			case GL_ONE_MINUS_CONSTANT_COLOR:
				return BlendFunction::ONE_MINUS_CONSTANT_COLOR;
			case GL_ONE_MINUS_DST_ALPHA:
				return BlendFunction::ONE_MINUS_DST_ALPHA;
			case GL_ONE_MINUS_DST_COLOR:
				return BlendFunction::ONE_MINUS_DST_COLOR;
			case GL_ONE_MINUS_SRC_ALPHA:
				return BlendFunction::ONE_MINUS_SRC_ALPHA;
			case GL_ONE_MINUS_SRC_COLOR:
				return BlendFunction::ONE_MINUS_SRC_COLOR;
			case GL_SRC_ALPHA:
				return BlendFunction::SRC_ALPHA;
			case GL_SRC_COLOR:
				return BlendFunction::SRC_COLOR;
			default:
				ASSERT(false);
				return BlendFunction::CONSTANT_ALPHA;
		}
	}

	int PolygonModeToInt(PolygonMode mode)
	{
		switch (mode)
		{
			case PolygonMode::FILL:
				return GL_FILL;
			case PolygonMode::LINE:
				return GL_LINE;
			case PolygonMode::POINT:
				return GL_POINT;
			default:
				ASSERT(false);
				return GL_FILL;
		}
	}

	PolygonMode IntToPolygonMode(int mode)
	{
		switch (mode)
		{
			case GL_FILL:
				return PolygonMode::FILL;
			case GL_LINE:
				return PolygonMode::LINE;
			case GL_POINT:
				return PolygonMode::POINT;
			default:
				ASSERT(false);
				return PolygonMode::FILL;
		}
	}

	int DrawModeToInt(DrawMode mode)
	{
		switch (mode)
		{
			case DrawMode::LINES:
				return GL_LINES;
			case DrawMode::LINE_STRIP:
				return GL_LINE_STRIP;
			case DrawMode::TRIANGLES:
				return GL_TRIANGLES;
			case DrawMode::TRIANGLE_FAN:
				return GL_TRIANGLE_FAN;
			case DrawMode::TRIANGLE_STRIP:
				return GL_TRIANGLE_STRIP;
			default:
				ASSERT(false);
				return GL_TRIANGLES;
		}
	}

	DrawMode IntToDrawMode(int mode)
	{
		switch (mode)
		{
			case GL_LINES:
				return DrawMode::LINES;
			case GL_LINE_STRIP:
				return DrawMode::LINE_STRIP;
			case GL_TRIANGLES:
				return DrawMode::TRIANGLES;
			case GL_TRIANGLE_FAN:
				return DrawMode::TRIANGLE_FAN;
			case GL_TRIANGLE_STRIP:
				return DrawMode::TRIANGLE_STRIP;
			default:
				ASSERT(false);
				return DrawMode::TRIANGLES;
		}
	}

	int CullFaceToInt(CullFace face)
	{
		switch (face)
		{
			case CullFace::BACK:
				return GL_BACK;
			case CullFace::FRONT:
				return GL_FRONT;
			case CullFace::FRONT_AND_BACK:
				return GL_FRONT_AND_BACK;
			default:
				ASSERT(false);
				return GL_BACK;
		}
	}

	CullFace IntToCullFace(int face)
	{
		switch (face)
		{
			case GL_BACK:
				return CullFace::BACK;
			case GL_FRONT:
				return CullFace::FRONT;
			case GL_FRONT_AND_BACK:
				return CullFace::FRONT_AND_BACK;
			default:
				ASSERT(false);
				return CullFace::BACK;
		}
	}

	int FrontFaceToInt(FrontFace face)
	{
		switch (face)
		{
			case FrontFace::CW:
				return GL_CW;
			case FrontFace::CCW:
				return GL_CCW;
			default:
				ASSERT(false);
				return GL_CW;
		}
	}

	FrontFace IntToFrontFace(int face)
	{
		switch (face)
		{
			case GL_CW:
				return FrontFace::CW;
			case GL_CCW:
				return FrontFace::CCW;
			default:
				ASSERT(false);
				return FrontFace::CW;
		}
	}

}