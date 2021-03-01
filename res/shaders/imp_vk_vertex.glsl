struct Vertex
{
    vec3 mPos;
    vec3 mNorm;
    vec2 mUV;
    vec4 mBoneWeights;
    ivec4 mBoneIds;
    int mBoneNum;
    int mBoneDebugId;
};