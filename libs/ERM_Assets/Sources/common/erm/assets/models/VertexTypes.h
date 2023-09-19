#pragma once

#include "erm/math/Types.h"

namespace erm {

using VertexType = float;
using IdType = u8;

using PositionVertex = vec<3, VertexType>;
using NormalVertex = vec<3, VertexType>;
using UVVertex = vec<2, VertexType>;
using BoneWeights = vec<4, VertexType>;
using BoneIds = vec<4, IdType>;
using BoneNum = u8;
using DebugBoneId = u8;

} // namespace erm
