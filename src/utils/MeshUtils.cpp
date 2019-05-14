#include "utils/MeshUtils.h"

#include "rendering/Mesh.h"
#include "rendering/VertexData.h"

namespace erm {
	
	Mesh MeshUtils::CreateTriangle(
		const glm::vec3& a /* = glm::vec3(-1.0f, -1.0f, 0.0f) */,
		const glm::vec3& b /* = glm::vec3(1.0f, -1.0f, 0.0f) */,
		const glm::vec3& c /* = glm::vec3(-1.0f, 1.0f, 0.0f) */
	) {
		Mesh mesh;
		
		mesh.mVerticesDataCount = 3;
		mesh.mVerticesData = static_cast<VertexData*>(malloc(sizeof(VertexData) * mesh.mVerticesDataCount));
		mesh.mVerticesData[0].mVertex = a;
		mesh.mVerticesData[0].mTextureVertex = TextureVertex(0.0f, 0.0f);
		mesh.mVerticesData[1].mVertex = b;
		mesh.mVerticesData[1].mTextureVertex = TextureVertex(0.0f, 1.0f);
		mesh.mVerticesData[2].mVertex = c;
		mesh.mVerticesData[2].mTextureVertex = TextureVertex(1.0f, 0.0f);
		
		mesh.mIndicesDataCount = 3;
		mesh.mIndicesData = static_cast<IndexData*>(malloc(sizeof(IndexData) * mesh.mIndicesDataCount));
		mesh.mIndicesData[0] = 0;
		mesh.mIndicesData[1] = 1;
		mesh.mIndicesData[2] = 2;
		
		mesh.Setup();
		
		return mesh;
	}
	
	Mesh MeshUtils::CreateSquare(
		float width /* = 2.0f */,
		float height /* = 2.0f */
	) {
		Mesh mesh;
		
		const float halfWidth = width*0.5f;
		const float halfHeight = height*0.5f;
		
		mesh.mVerticesDataCount = 4;
		mesh.mVerticesData = static_cast<VertexData*>(malloc(sizeof(VertexData) * mesh.mVerticesDataCount));
		mesh.mVerticesData[0].mVertex = Vertex(-halfWidth, -halfHeight, 0.0f);
		mesh.mVerticesData[0].mTextureVertex = TextureVertex(0.0f, 0.0f);
		mesh.mVerticesData[1].mVertex = Vertex(halfWidth, -halfHeight, 0.0f);
		mesh.mVerticesData[1].mTextureVertex = TextureVertex(1.0f, 0.0f);
		mesh.mVerticesData[2].mVertex = Vertex(-halfWidth, halfHeight, 0.0f);
		mesh.mVerticesData[2].mTextureVertex = TextureVertex(0.0f, 1.0f);
		mesh.mVerticesData[3].mVertex = Vertex(halfWidth, halfHeight, 0.0f);
		mesh.mVerticesData[3].mTextureVertex = TextureVertex(1.0f, 1.0f);
		
		mesh.mIndicesDataCount = 6;
		mesh.mIndicesData = static_cast<IndexData*>(malloc(sizeof(IndexData) * mesh.mIndicesDataCount));
		mesh.mIndicesData[0] = 0;
		mesh.mIndicesData[1] = 1;
		mesh.mIndicesData[2] = 2;
		mesh.mIndicesData[3] = 2;
		mesh.mIndicesData[4] = 1;
		mesh.mIndicesData[5] = 3;
		
		mesh.Setup();
		
		return mesh;
	}
	
	Mesh MeshUtils::CreateCube(
		float sizeX /* = 2.0f */,
		float sizeY /* = 2.0f */,
		float sizeZ /* = 2.0f */
	) {
		const float halfX = sizeX*0.5f;
		const float halfY = sizeY*0.5f;
		const float halfZ = sizeZ*0.5f;
		
		Mesh mesh;
		
		mesh.mVerticesDataCount = 6 * 4;
		mesh.mVerticesData = static_cast<VertexData*>(malloc(sizeof(VertexData) * mesh.mVerticesDataCount));
		
		int vertIndex = 0;
		
		// Front
		mesh.mVerticesData[vertIndex].mVertex = Vertex(-halfX, -halfY, -halfZ);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(0.0f, 0.0f);
		mesh.mVerticesData[vertIndex].mVertex = Vertex(halfX, -halfY, -halfZ);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(1.0f, 0.0f);
		mesh.mVerticesData[vertIndex].mVertex = Vertex(-halfX, halfY, -halfZ);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(0.0f, 1.0f);
		mesh.mVerticesData[vertIndex].mVertex = Vertex(halfX, halfY, -halfZ);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(1.0f, 1.0f);
		
		// Back
		mesh.mVerticesData[vertIndex].mVertex = Vertex(halfX, -halfY, halfZ);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(0.0f, 0.0f);
		mesh.mVerticesData[vertIndex].mVertex = Vertex(-halfX, -halfY, halfZ);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(1.0f, 0.0f);
		mesh.mVerticesData[vertIndex].mVertex = Vertex(halfX, halfY, halfZ);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(0.0f, 1.0f);
		mesh.mVerticesData[vertIndex].mVertex = Vertex(-halfX, halfY, halfZ);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(1.0f, 1.0f);
		
		// Top
		mesh.mVerticesData[vertIndex].mVertex = Vertex(-halfX, halfY, -halfZ);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(0.0f, 0.0f);
		mesh.mVerticesData[vertIndex].mVertex = Vertex(halfX, halfY, -halfZ);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(1.0f, 0.0f);
		mesh.mVerticesData[vertIndex].mVertex = Vertex(-halfX, halfY, halfZ);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(0.0f, 1.0f);
		mesh.mVerticesData[vertIndex].mVertex = Vertex(halfX, halfY, halfZ);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(1.0f, 1.0f);
		
		// Bottom
		mesh.mVerticesData[vertIndex].mVertex = Vertex(-halfX, -halfY, halfZ);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(0.0f, 0.0f);
		mesh.mVerticesData[vertIndex].mVertex = Vertex(halfX, -halfY, halfZ);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(1.0f, 0.0f);
		mesh.mVerticesData[vertIndex].mVertex = Vertex(-halfX, -halfY, -halfZ);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(0.0f, 1.0f);
		mesh.mVerticesData[vertIndex].mVertex = Vertex(halfX, -halfY, -halfZ);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(1.0f, 1.0f);
		
		// Right
		mesh.mVerticesData[vertIndex].mVertex = Vertex(halfX, -halfY, -halfZ);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(0.0f, 0.0f);
		mesh.mVerticesData[vertIndex].mVertex = Vertex(halfX, -halfY, halfZ);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(1.0f, 0.0f);
		mesh.mVerticesData[vertIndex].mVertex = Vertex(halfX, halfY, -halfZ);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(0.0f, 1.0f);
		mesh.mVerticesData[vertIndex].mVertex = Vertex(halfX, halfY, halfZ);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(1.0f, 1.0f);
		
		// Left
		mesh.mVerticesData[vertIndex].mVertex = Vertex(-halfX, -halfY, halfZ);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(0.0f, 0.0f);
		mesh.mVerticesData[vertIndex].mVertex = Vertex(-halfX, -halfY, -halfZ);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(1.0f, 0.0f);
		mesh.mVerticesData[vertIndex].mVertex = Vertex(-halfX, halfY, halfZ);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(0.0f, 1.0f);
		mesh.mVerticesData[vertIndex].mVertex = Vertex(-halfX, halfY, -halfZ);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(1.0f, 1.0f);
		
		// Indices
		mesh.mIndicesDataCount = 36;
		mesh.mIndicesData = static_cast<IndexData*>(malloc(sizeof(IndexData) * mesh.mIndicesDataCount));
		
		for (int i = 0; i < 6; ++i)
		{
			const int offset = i * 4;
			mesh.mIndicesData[i * 6] = offset;
			mesh.mIndicesData[(i * 6) + 1] = offset + 1;
			mesh.mIndicesData[(i * 6) + 2] = offset + 2;
			mesh.mIndicesData[(i * 6) + 3] = offset + 2;
			mesh.mIndicesData[(i * 6) + 4] = offset + 1;
			mesh.mIndicesData[(i * 6) + 5] = offset + 3;
		}
		
		mesh.Setup();
		
		return mesh;
	}
	
	Mesh MeshUtils::CreateSpike(
		float sizeX /* = 2.0f */,
		float sizeY /* = 2.0f */,
		float sizeZ /* = 2.0f */
	) {
		const float halfX = sizeX*0.5f;
		const float halfY = sizeY*0.5f;
		const float halfZ = sizeZ*0.5f;
		
		Mesh mesh;
		
		mesh.mVerticesDataCount = 4 + 4 * 3;
		mesh.mVerticesData = static_cast<VertexData*>(malloc(sizeof(VertexData) * mesh.mVerticesDataCount));
		
		int vertIndex = 0;
		
		// Bottom
		mesh.mVerticesData[vertIndex].mVertex = Vertex(-halfX, -halfY, halfZ);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(0.0f, 0.0f);
		mesh.mVerticesData[vertIndex].mVertex = Vertex(halfX, -halfY, halfZ);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(1.0f, 0.0f);
		mesh.mVerticesData[vertIndex].mVertex = Vertex(-halfX, -halfY, -halfZ);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(0.0f, 1.0f);
		mesh.mVerticesData[vertIndex].mVertex = Vertex(halfX, -halfY, -halfZ);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(1.0f, 1.0f);
		
		// Front
		mesh.mVerticesData[vertIndex].mVertex = Vertex(-halfX, -halfY, -halfZ);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(0.0f, 0.0f);
		mesh.mVerticesData[vertIndex].mVertex = Vertex(halfX, -halfY, -halfZ);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(0.5f, 0.0f);
		mesh.mVerticesData[vertIndex].mVertex = Vertex(0.0f, halfY, 0.0f);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(0.0f, 1.0f);
		
		// Right
		mesh.mVerticesData[vertIndex].mVertex = Vertex(halfX, -halfY, -halfZ);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(0.0f, 0.0f);
		mesh.mVerticesData[vertIndex].mVertex = Vertex(halfX, -halfY, halfZ);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(1.0f, 0.0f);
		mesh.mVerticesData[vertIndex].mVertex = Vertex(0.0f, halfY, 0.0f);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(1.0f, 1.0f);
		
		// Back
		mesh.mVerticesData[vertIndex].mVertex = Vertex(halfX, -halfY, halfZ);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(0.0f, 0.0f);
		mesh.mVerticesData[vertIndex].mVertex = Vertex(-halfX, -halfY, halfZ);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(1.0f, 0.0f);
		mesh.mVerticesData[vertIndex].mVertex = Vertex(0.0f, halfY, 0.0f);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(0.0f, 1.0f);
		
		// Left
		mesh.mVerticesData[vertIndex].mVertex = Vertex(-halfX, -halfY, halfZ);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(0.0f, 0.0f);
		mesh.mVerticesData[vertIndex].mVertex = Vertex(-halfX, -halfY, -halfZ);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(1.0f, 0.0f);
		mesh.mVerticesData[vertIndex].mVertex = Vertex(0.0f, halfY, 0.0f);
		mesh.mVerticesData[vertIndex++].mTextureVertex = TextureVertex(1.0f, 1.0f);
		
		// Indices
		mesh.mIndicesDataCount = 18;
		mesh.mIndicesData = static_cast<IndexData*>(malloc(sizeof(IndexData) * mesh.mIndicesDataCount));
		
		mesh.mIndicesData[0] = 0;
		mesh.mIndicesData[1] = 1;
		mesh.mIndicesData[2] = 2;
		mesh.mIndicesData[3] = 2;
		mesh.mIndicesData[4] = 1;
		mesh.mIndicesData[5] = 3;
		mesh.mIndicesData[6] = 4;
		mesh.mIndicesData[7] = 5;
		mesh.mIndicesData[8] = 6;
		mesh.mIndicesData[9] = 7;
		mesh.mIndicesData[10] = 8;
		mesh.mIndicesData[11] = 9;
		mesh.mIndicesData[12] = 10;
		mesh.mIndicesData[13] = 11;
		mesh.mIndicesData[14] = 12;
		mesh.mIndicesData[15] = 13;
		mesh.mIndicesData[16] = 14;
		mesh.mIndicesData[17] = 15;
		
		mesh.Setup();
		
		return mesh;
	}
	
	Mesh MeshUtils::CreateSphere(
		float radius /* = 1.0f */,
		int sectors /* = 10 */,
		int rings /* = 10 */
	) {
		Mesh mesh;
		
		mesh.mVerticesDataCount = (sectors + 1) * (rings + 1);
		mesh.mVerticesData = static_cast<VertexData*>(malloc(sizeof(VertexData) * mesh.mVerticesDataCount));
		
		mesh.mIndicesDataCount = sectors * rings * 6 - sectors * 6;
		mesh.mIndicesData = static_cast<IndexData*>(malloc(sizeof(IndexData) * mesh.mIndicesDataCount));
		
		float x, y, z, xy;
		float s, t;
		
		float sectorStep = static_cast<float>(2.0f * M_PI / sectors);
		float stackStep = static_cast<float>(M_PI / rings);
		float sectorAngle, stackAngle;
		int index = 0;
		
		for (int i = 0; i <= rings; ++i)
		{
			stackAngle = static_cast<float>(M_PI / 2.0f - i * stackStep);
			xy = radius * cosf(stackAngle);
			z = radius * sinf(stackAngle);
			
			for (int j = 0; j <= sectors; ++j)
			{
				sectorAngle = j * sectorStep;
				
				x = xy * cosf(sectorAngle);
				y = xy * sinf(sectorAngle);
				mesh.mVerticesData[index].mVertex = Vertex(x, y, z);
				
				s = (float)j / sectors;
				t = (float)i / rings;
				mesh.mVerticesData[index].mTextureVertex = TextureVertex(s, t);
				
				index++;
			}
		}
		
		int k1, k2;
		index = 0;
		
		for (int i = 0; i < rings; ++i)
		{
			k1 = i * (sectors + 1);
			k2 = k1 + sectors + 1;
			
			for (int j = 0; j < sectors; ++j, ++k1, ++k2)
			{
				if (i != 0)
				{
					mesh.mIndicesData[index++] = k1;
					mesh.mIndicesData[index++] = k2;
					mesh.mIndicesData[index++] = k1 + 1;
				}
				
				if (i != (rings-1))
				{
					mesh.mIndicesData[index++] = k1 + 1;
					mesh.mIndicesData[index++] = k2;
					mesh.mIndicesData[index++] = k2 + 1;
				}
			}
		}
		
		mesh.Setup();
		
		return mesh;
	}
	
}
