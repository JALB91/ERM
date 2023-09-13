#pragma once

namespace erm {

struct CubeMap;

template<typename T>
class GPUImage;

using GPUCubeMap = GPUImage<CubeMap>;

}
