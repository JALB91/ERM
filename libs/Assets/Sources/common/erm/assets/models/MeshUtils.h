#pragma once

#include <erm/math/Types.h>

namespace erm {
struct Mesh;
} // namespace erm

namespace erm::MeshUtils {

extern Mesh CreateTriangle(
	const vec3& a = vec3(-1.0f, -1.0f, 0.0f),
	const vec3& b = vec3(1.0f, -1.0f, 0.0f),
	const vec3& c = vec3(-1.0f, 1.0f, 0.0f));

extern Mesh CreateSquare(
	float width = 1.0f,
	float height = 1.0f);

extern Mesh CreateCube(
	float sizeX = 1.0f,
	float sizeY = 1.0f,
	float sizeZ = 1.0f);

extern Mesh CreateSpike(
	float sizeX = 1.0f,
	float sizeY = 1.0f,
	float sizeZ = 1.0f,
	u8 boneId = 0);

extern Mesh CreateSphere(
	float radius = 1.0f,
	u8 stackCount = 10,
	u8 sectorCount = 10);

extern Mesh CreateGrid(
	u8 sizeX = 100,
	u8 sizeY = 100,
	float width = 1.0f,
	float height = 1.0f);

} // namespace erm::MeshUtils
