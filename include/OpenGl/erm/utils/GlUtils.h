#pragma once

#include "erm/rendering/enums/BlendFunction.h"
#include "erm/rendering/enums/DepthFunction.h"
#include "erm/rendering/enums/PolygonMode.h"
#include "erm/rendering/enums/DrawMode.h"
#include "erm/rendering/enums/CullFace.h"
#include "erm/rendering/enums/FrontFace.h"

namespace erm {

	extern int DepthFunctionToInt(DepthFunction function);
	extern DepthFunction IntToDepthFunction(int function);

	extern int BlendFunctionToInt(BlendFunction function);
	extern BlendFunction IntToBlendFunction(int function);

	extern int PolygonModeToInt(PolygonMode mode);
	extern PolygonMode IntToPolygonMode(int mode);

	extern int DrawModeToInt(DrawMode mode);
	extern DrawMode IntToDrawMode(int mode);

	extern int CullFaceToInt(CullFace face);
	extern CullFace IntToCullFace(int face);

	extern int FrontFaceToInt(FrontFace face);
	extern FrontFace IntToFrontFace(int face);

}