#pragma once

#include "rendering/Mesh.h"

#include <glm/glm.hpp>

namespace erm {
	
	class MeshUtils
	{
	public:
		static Mesh CreateTriangle(
			const glm::vec3& a = glm::vec3(-1.0f, -1.0f, 0.0f),
			const glm::vec3& b = glm::vec3(1.0f, -1.0f, 0.0f),
			const glm::vec3& c = glm::vec3(-1.0f, 1.0f, 0.0f)
		);
		static Mesh CreateSquare(
			float width = 2.0f,
			float height = 2.0f
		);
		static Mesh CreateCube(
			float sizeX = 2.0f,
			float sizeY = 2.0f,
			float sizeZ = 2.0f
		);
		static Mesh CreateSpike(
			float sizeX = 2.0f,
			float sizeY = 2.0f,
			float sizeZ = 2.0f
		);
		static Mesh CreateSphere(
			float radius = 1.0f,
			int stackCount = 10,
			int sectorCount = 10
		);
		static Mesh ParseModel(const char* path);
	};
	
}
