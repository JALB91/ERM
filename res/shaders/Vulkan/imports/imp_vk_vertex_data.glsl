#if defined(ERM_RTX)
struct Vertex
{
    vec3 mPosition;
    vec3 mNormal;
    vec2 mTexCoord;
    vec4 mBoneWeights;
    ivec4 mBoneIds;
    int mBoneNum;
    int mBoneDebugId;
};
#elif defined(ERM_RASTER)
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in vec4 inBoneWeights;
layout (location = 4) in ivec4 inBoneIds;
layout (location = 5) in int inBoneNum;
layout (location = 6) in int inBoneDebugId;
#endif