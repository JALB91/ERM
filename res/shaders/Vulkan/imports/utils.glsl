#ifndef UTILS_GLSL
#define UTILS_GLSL

float depthFromDist(float dist, float zNear, float zFar)
{
    return ((1.0 / dist) - (1.0 / zNear)) / ((1.0 / zFar) - (1.0 / zNear));
}

float linearizeDepth(float depth, float zNear, float zFar) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * zNear * zFar) / (zFar + zNear - z * (zFar - zNear));	
}

#endif