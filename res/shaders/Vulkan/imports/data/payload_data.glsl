#ifndef PAYLOAD_DATA_GLSL
#define PAYLOAD_DATA_GLSL

struct HitPayload
{
	vec3 mHitValue;
	vec3 mAttenuation;
	vec3 mOrigin;
	vec3 mDir;
	int mDepth;
	bool mDone;
	bool mSkip;
};

#endif