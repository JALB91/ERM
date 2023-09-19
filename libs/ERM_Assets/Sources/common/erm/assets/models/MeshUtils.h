#pragma once

#include <erm/math/Types.h>

namespace erm {
struct Mesh;
} // namespace erm

namespace erm::MeshUtils {

extern Mesh createTriangle(
	const vec3& a = vec3(-1.0f, -1.0f, 0.0f),
	const vec3& b = vec3(1.0f, -1.0f, 0.0f),
	const vec3& c = vec3(-1.0f, 1.0f, 0.0f));

extern Mesh createSquare(
	float width = 1.0f,
	float height = 1.0f);

extern Mesh createCube(
	float sizeX = 1.0f,
	float sizeY = 1.0f,
	float sizeZ = 1.0f);

extern Mesh createSpike(
	float sizeX = 1.0f,
	float sizeY = 1.0f,
	float sizeZ = 1.0f,
	u32 boneId = 0);

extern Mesh createSphere(
	float radius = 1.0f,
	u32 stackCount = 10,
	u32 sectorCount = 10);

extern Mesh createGrid(
	u32 sizeX = 100,
	u32 sizeY = 100,
	float width = 1.0f,
	float height = 1.0f);

} // namespace erm::MeshUtils
