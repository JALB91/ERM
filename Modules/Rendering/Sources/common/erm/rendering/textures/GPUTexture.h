#pragma once

namespace erm {

struct Texture;

template<typename T>
class GPUImage;

using GPUTexture = GPUImage<Texture>;

}
