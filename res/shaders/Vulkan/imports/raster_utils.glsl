#ifndef RASTER_UTILS_GLSL
#define RASTER_UTILS_GLSL

vec3 lit(vec3 l, vec3 n, vec3 v, vec3 mDiffuse, vec3 lDiffuse) {
	vec3 r_l = reflect(-l, n);
	float s = clamp(100.0 * dot(r_l, v) - 100.0, 0.0, 1.0);
	return mix(mDiffuse, lDiffuse, s);
}

#endif