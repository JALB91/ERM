#include "utils/ModelUtils.h"
#include "utils/MeshUtils.h"

#include "rendering/Mesh.h"
#include "rendering/Model.h"
#include "rendering/VertexData.h"

namespace erm {
	
	Model ModelUtils::CreateCube(
		float sizeX /* = 2.0f */,
		float sizeY /* = 2.0f */,
		float sizeZ /* = 2.0f */
	)
	{
		std::vector<Mesh> meshes;
		
		const float halfX = sizeX*0.5f;
		const float halfY = sizeY*0.5f;
		const float halfZ = sizeZ*0.5f;
		
		// Front
		{
			Mesh face = MeshUtils::CreateSquare(sizeX, sizeY);
			face.Translate(glm::vec3(0.0f, 0.0f, -halfZ));
			meshes.emplace_back(std::move(face));
		}
		
		// Back
		{
			Mesh face = MeshUtils::CreateSquare(sizeX, sizeY);
			face.Translate(glm::vec3(0.0f, 0.0f, halfZ));
			face.Rotate(glm::vec3(0.0f, M_PI, 0.0f));
			meshes.emplace_back(std::move(face));
		}
		
		// Right
		{
			Mesh face = MeshUtils::CreateSquare(sizeZ, sizeY);
			face.Translate(glm::vec3(halfX, 0.0f, 0.0f));
			face.Rotate(glm::vec3(0.0f, -M_PI*0.5f, 0.0f));
			meshes.emplace_back(std::move(face));
		}
		
		// Left
		{
			Mesh face = MeshUtils::CreateSquare(sizeZ, sizeY);
			face.Translate(glm::vec3(-halfX, 0.0f, 0.0f));
			face.Rotate(glm::vec3(0.0f, M_PI*0.5f, 0.0f));
			meshes.emplace_back(std::move(face));
		}
		
		// Top
		{
			Mesh face = MeshUtils::CreateSquare(sizeX, sizeZ);
			face.Translate(glm::vec3(0.0f, halfY, 0.0f));
			face.Rotate(glm::vec3(M_PI*0.5f, 0.0f, 0.0f));
			meshes.emplace_back(std::move(face));
		}
		
		// Bottom
		{
			Mesh face = MeshUtils::CreateSquare(sizeX, sizeZ);
			face.Translate(glm::vec3(0.0f, -halfY, 0.0f));
			face.Rotate(glm::vec3(-M_PI*0.5f, 0.0f, 0.0f));
			meshes.emplace_back(std::move(face));
		}
		
		return Model(std::move(meshes));
	}
	
	Model ModelUtils::CreateSpike(
		float sizeX /* = 2.0f */,
		float sizeY /* = 2.0f */,
		float sizeZ /* = 2.0f */
	)
	{
		std::vector<Mesh> meshes;
		
		const float halfX = sizeX*0.5f;
		const float halfY = sizeY*0.5f;
		const float halfZ = sizeZ*0.5f;
		
		// Base
		{
			Mesh face = MeshUtils::CreateSquare(sizeX, sizeZ);
			face.Translate(glm::vec3(0.0f, -halfY, 0.0f));
			face.Rotate(glm::vec3(-M_PI*0.5f, 0.0f, 0.0f));
			meshes.push_back(std::move(face));
		}
		
		// Right
		{
			Mesh face = MeshUtils::CreateTriangle(
				glm::vec3(halfX, -halfY, -halfZ),
				glm::vec3(halfX, -halfY, halfZ),
				glm::vec3(0.0f, halfY, 0.0f)
			);
			meshes.push_back(std::move(face));
		}
		
		// Left
		{
			Mesh face = MeshUtils::CreateTriangle(
				glm::vec3(-halfX, -halfY, halfZ),
				glm::vec3(-halfX, -halfY, -halfZ),
				glm::vec3(0.0f, halfY, 0.0f)
			);
			meshes.push_back(std::move(face));
		}
		
		// Front
		{
			Mesh face = MeshUtils::CreateTriangle(
				glm::vec3(-halfX, -halfY, -halfZ),
				glm::vec3(halfX, -halfY, -halfZ),
				glm::vec3(0.0f, halfY, 0.0f)
			);
			meshes.push_back(std::move(face));
		}
		
		// Back
		{
			Mesh face = MeshUtils::CreateTriangle(
				glm::vec3(halfX, -halfY, halfZ),
				glm::vec3(-halfX, -halfY, halfZ),
				glm::vec3(0.0f, halfY, 0.0f)
			);
			meshes.push_back(std::move(face));
		}
		
		return Model(std::move(meshes));
	}

	Model ModelUtils::CreateSphere(
		float radius /* = 1.0f */,
		int stackCount /* = 10 */,
		int sectorCount /* = 10 */
	)
	{
		Mesh mesh;
		
		mesh.mVerticesDataCount = (stackCount + 1) * (sectorCount + 1);
		mesh.mVerticesData = static_cast<VertexData*>(malloc(sizeof(VertexData) * mesh.mVerticesDataCount));
		
		float s, t;
		float x, y, z, xy;
		float sectorAngle, stackAngle;
		float stackStep = M_PI / static_cast<float>(stackCount);
		float sectorStep = 2.0f * M_PI / static_cast<float>(sectorCount);
		
		for (int i = 0; i <= stackCount; ++i)
		{
			z = radius * sinf(stackAngle);
			xy = radius * cosf(stackAngle);
			stackAngle = M_PI / 2 - static_cast<float>(i * stackStep);
			
			for (int j = 0; j <= sectorCount; ++j)
			{
				sectorAngle = j * sectorStep;
				
				x = xy * cosf(sectorAngle);
				y = xy * sinf(sectorAngle);
				mesh.mVerticesData[i+j].mVertex = Vertex(x, y, z);
				
				s = static_cast<float>(j) / sectorCount;
				t = static_cast<float>(i) / stackCount;
				mesh.mVerticesData[i+j].mTextureVertex = TextureVertex(s, t);
			}
		}
		
		mesh.mIndicesDataCount = (stackCount * sectorCount * 2) - 2 * stackCount;
		mesh.mIndicesData = static_cast<IndexData*>(malloc(sizeof(IndexType) * mesh.mIndicesDataCount));
		
		int k1, k2;
		int index = 0;
		
		for (int i = 0; i < stackCount; ++i)
		{
			k1 = i * (sectorCount + 1);
			k2 = k1 + sectorCount + 1;
			
			for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
			{
				if (i != 0)
				{
//					mesh.mIndicesData[index++] = IndexData(k1, k2, k1 + 1);
//					mesh.mIndicesData[index++] = IndexData(k2, k1, k1 + 1);
				}
				
				if (i != (stackCount-1))
				{
//					mesh.mIndicesData[index++] = IndexData(k1 + 1, k2, k2 + 1);
//					mesh.mIndicesData[index++] = IndexData(k2, k1 + 1, k2 + 1);
				}
			}
		}
		
		mesh.Setup();
		
		return Model({ std::move(mesh) });
	}
	
}
