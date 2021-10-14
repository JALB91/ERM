#pragma once

#include "erm/math/vec.h"

namespace erm {
class Mesh;
} // namespace erm

namespace erm::MeshUtils {

extern Mesh CreateTriangle(
	const math::vec3& a = math::vec3(-1.0f, -1.0f, 0.0f),
	const math::vec3& b = math::vec3(1.0f, -1.0f, 0.0f),
	const math::vec3& c = math::vec3(-1.0f, 1.0f, 0.0f));

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
	int boneId = 0);
extern Mesh CreateSphere(
	float radius = 1.0f,
	unsigned int stackCount = 10,
	unsigned int sectorCount = 10);

extern Mesh CreateGrid(
	unsigned int sizeX = 100,
	unsigned int sizeY = 100,
	float width = 1.0f,
	float height = 1.0f);

} // namespace erm::MeshUtils
