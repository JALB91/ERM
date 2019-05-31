#pragma once

#include <glm/glm.hpp>

namespace erm {
	
	class Mesh;
	
	class MeshUtils
	{
	public:
		static Mesh CreateTriangle(
			const glm::vec3& a = glm::vec3(-1.0f, -1.0f, 0.0f),
			const glm::vec3& b = glm::vec3(1.0f, -1.0f, 0.0f),
			const glm::vec3& c = glm::vec3(-1.0f, 1.0f, 0.0f)
		);
		static Mesh CreateSquare(
			float width = 1.0f,
			float height = 1.0f
		);
		static Mesh CreateCube(
			float sizeX = 1.0f,
			float sizeY = 1.0f,
			float sizeZ = 1.0f
		);
		static Mesh CreateSpike(
			float sizeX = 1.0f,
			float sizeY = 1.0f,
			float sizeZ = 1.0f
		);
		static Mesh CreateSphere(
			float radius = 1.0f,
			int stackCount = 10,
			int sectorCount = 10
		);
	};
	
}
