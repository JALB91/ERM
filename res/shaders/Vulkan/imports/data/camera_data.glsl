#ifndef CAMERA_DATA
#define CAMERA_DATA

struct Camera
{
	vec3 position;
	float zNear;
    float zFar;
    float fov;
};

#endif