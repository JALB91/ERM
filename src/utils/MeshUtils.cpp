#include "utils/MeshUtils.h"

#include "rendering/Mesh.h"
#include "rendering/VertexData.h"

namespace erm {
	
	Mesh MeshUtils::CreateTriangle(
		const math::vec3& a /* = math::vec3(-1.0f, -1.0f, 0.0f) */,
		const math::vec3& b /* = math::vec3(1.0f, -1.0f, 0.0f) */,
		const math::vec3& c /* = math::vec3(-1.0f, 1.0f, 0.0f) */
	) {
		const unsigned int verticesCount = 3;
		VertexData* vertices = new VertexData[verticesCount];
		vertices[0].mPositionVertex = a;
		vertices[0].mNormalVertex = NormalVertex(0.0f, 0.0f, -1.0f);
		vertices[1].mPositionVertex = b;
		vertices[1].mNormalVertex = NormalVertex(0.0f, 0.0f, -1.0f);
		vertices[2].mPositionVertex = c;
		vertices[2].mNormalVertex = NormalVertex(0.0f, 0.0f, -1.0f);
		
		const unsigned int indicesCount = 3;
		IndexData* indices = new IndexData[indicesCount];
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;
		
		Mesh mesh (DrawMode::TRIANGLES, vertices, verticesCount, indices, indicesCount);
		mesh.Setup();
		return mesh;
	}
	
	Mesh MeshUtils::CreateSquare(
		float width /* = 1.0f */,
		float height /* = 1.0f */
	) {
		const float halfWidth = width*0.5f;
		const float halfHeight = height*0.5f;
		
		const unsigned int verticesCount = 4;
		VertexData* vertices = new VertexData[verticesCount];
		vertices[0].mPositionVertex = PositionVertex(-halfWidth, -halfHeight, 0.0f);
		vertices[0].mNormalVertex = NormalVertex(0.0f, 0.0f, -1.0f);
		vertices[1].mPositionVertex = PositionVertex(halfWidth, -halfHeight, 0.0f);
		vertices[1].mNormalVertex = NormalVertex(0.0f, 0.0f, -1.0f);
		vertices[2].mPositionVertex = PositionVertex(-halfWidth, halfHeight, 0.0f);
		vertices[2].mNormalVertex = NormalVertex(0.0f, 0.0f, -1.0f);
		vertices[3].mPositionVertex = PositionVertex(halfWidth, halfHeight, 0.0f);
		vertices[3].mNormalVertex = NormalVertex(0.0f, 0.0f, -1.0f);
		
		const unsigned int indicesCount = 6;
		IndexData* indices = new IndexData[indicesCount];
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;
		indices[3] = 2;
		indices[4] = 1;
		indices[5] = 3;
		
		Mesh mesh (DrawMode::TRIANGLES, vertices, verticesCount, indices, indicesCount);
		mesh.Setup();
		return mesh;
	}
	
	Mesh MeshUtils::CreateCube(
		float sizeX /* = 1.0f */,
		float sizeY /* = 1.0f */,
		float sizeZ /* = 1.0f */
	) {
		const float halfX = sizeX*0.5f;
		const float halfY = sizeY*0.5f;
		const float halfZ = sizeZ*0.5f;
		
		const unsigned int verticesCount = 6 * 4;
		VertexData* vertices = new VertexData[verticesCount];
		
		int vertIndex = 0;
		
		// Front
		vertices[vertIndex].mPositionVertex = PositionVertex(-halfX, -halfY, -halfZ);
		vertices[vertIndex].mNormalVertex = NormalVertex(0.0f, 0.0f, -1.0f);
		vertices[vertIndex++].mUVVertex = UVVertex(0.0f, 0.0f);
		vertices[vertIndex].mPositionVertex = PositionVertex(halfX, -halfY, -halfZ);
		vertices[vertIndex].mNormalVertex = NormalVertex(0.0f, 0.0f, -1.0f);
		vertices[vertIndex++].mUVVertex = UVVertex(1.0f, 0.0f);
		vertices[vertIndex].mPositionVertex = PositionVertex(-halfX, halfY, -halfZ);
		vertices[vertIndex].mNormalVertex = NormalVertex(0.0f, 0.0f, -1.0f);
		vertices[vertIndex++].mUVVertex = UVVertex(0.0f, 1.0f);
		vertices[vertIndex].mPositionVertex = PositionVertex(halfX, halfY, -halfZ);
		vertices[vertIndex].mNormalVertex = NormalVertex(0.0f, 0.0f, -1.0f);
		vertices[vertIndex++].mUVVertex = UVVertex(1.0f, 1.0f);
		
		// Back
		vertices[vertIndex].mPositionVertex = PositionVertex(halfX, -halfY, halfZ);
		vertices[vertIndex].mNormalVertex = NormalVertex(0.0f, 0.0f, 1.0f);
		vertices[vertIndex++].mUVVertex = UVVertex(0.0f, 0.0f);
		vertices[vertIndex].mPositionVertex = PositionVertex(-halfX, -halfY, halfZ);
		vertices[vertIndex].mNormalVertex = NormalVertex(0.0f, 0.0f, 1.0f);
		vertices[vertIndex++].mUVVertex = UVVertex(1.0f, 0.0f);
		vertices[vertIndex].mPositionVertex = PositionVertex(halfX, halfY, halfZ);
		vertices[vertIndex].mNormalVertex = NormalVertex(0.0f, 0.0f, 1.0f);
		vertices[vertIndex++].mUVVertex = UVVertex(0.0f, 1.0f);
		vertices[vertIndex].mPositionVertex = PositionVertex(-halfX, halfY, halfZ);
		vertices[vertIndex].mNormalVertex = NormalVertex(0.0f, 0.0f, 1.0f);
		vertices[vertIndex++].mUVVertex = UVVertex(1.0f, 1.0f);
		
		// Top
		vertices[vertIndex].mPositionVertex = PositionVertex(-halfX, halfY, -halfZ);
		vertices[vertIndex].mNormalVertex = NormalVertex(0.0f, 1.0f, 0.0f);
		vertices[vertIndex++].mUVVertex = UVVertex(0.0f, 0.0f);
		vertices[vertIndex].mPositionVertex = PositionVertex(halfX, halfY, -halfZ);
		vertices[vertIndex].mNormalVertex = NormalVertex(0.0f, 1.0f, 0.0f);
		vertices[vertIndex++].mUVVertex = UVVertex(1.0f, 0.0f);
		vertices[vertIndex].mPositionVertex = PositionVertex(-halfX, halfY, halfZ);
		vertices[vertIndex].mNormalVertex = NormalVertex(0.0f, 1.0f, 0.0f);
		vertices[vertIndex++].mUVVertex = UVVertex(0.0f, 1.0f);
		vertices[vertIndex].mPositionVertex = PositionVertex(halfX, halfY, halfZ);
		vertices[vertIndex].mNormalVertex = NormalVertex(0.0f, 1.0f, 0.0f);
		vertices[vertIndex++].mUVVertex = UVVertex(1.0f, 1.0f);
		
		// Bottom
		vertices[vertIndex].mPositionVertex = PositionVertex(-halfX, -halfY, halfZ);
		vertices[vertIndex].mNormalVertex = NormalVertex(0.0f, -1.0f, 0.0f);
		vertices[vertIndex++].mUVVertex = UVVertex(0.0f, 0.0f);
		vertices[vertIndex].mPositionVertex = PositionVertex(halfX, -halfY, halfZ);
		vertices[vertIndex].mNormalVertex = NormalVertex(0.0f, -1.0f, 0.0f);
		vertices[vertIndex++].mUVVertex = UVVertex(1.0f, 0.0f);
		vertices[vertIndex].mPositionVertex = PositionVertex(-halfX, -halfY, -halfZ);
		vertices[vertIndex].mNormalVertex = NormalVertex(0.0f, -1.0f, 0.0f);
		vertices[vertIndex++].mUVVertex = UVVertex(0.0f, 1.0f);
		vertices[vertIndex].mPositionVertex = PositionVertex(halfX, -halfY, -halfZ);
		vertices[vertIndex].mNormalVertex = NormalVertex(0.0f, -1.0f, 0.0f);
		vertices[vertIndex++].mUVVertex = UVVertex(1.0f, 1.0f);
		
		// Right
		vertices[vertIndex].mPositionVertex = PositionVertex(halfX, -halfY, -halfZ);
		vertices[vertIndex].mNormalVertex = NormalVertex(1.0f, 0.0f, 0.0);
		vertices[vertIndex++].mUVVertex = UVVertex(0.0f, 0.0f);
		vertices[vertIndex].mPositionVertex = PositionVertex(halfX, -halfY, halfZ);
		vertices[vertIndex].mNormalVertex = NormalVertex(1.0f, 0.0f, 0.0);
		vertices[vertIndex++].mUVVertex = UVVertex(1.0f, 0.0f);
		vertices[vertIndex].mPositionVertex = PositionVertex(halfX, halfY, -halfZ);
		vertices[vertIndex].mNormalVertex = NormalVertex(1.0f, 0.0f, 0.0);
		vertices[vertIndex++].mUVVertex = UVVertex(0.0f, 1.0f);
		vertices[vertIndex].mPositionVertex = PositionVertex(halfX, halfY, halfZ);
		vertices[vertIndex].mNormalVertex = NormalVertex(1.0f, 0.0f, 0.0);
		vertices[vertIndex++].mUVVertex = UVVertex(1.0f, 1.0f);
		
		// Left
		vertices[vertIndex].mPositionVertex = PositionVertex(-halfX, -halfY, halfZ);
		vertices[vertIndex].mNormalVertex = NormalVertex(-1.0f, 0.0f, 0.0);
		vertices[vertIndex++].mUVVertex = UVVertex(0.0f, 0.0f);
		vertices[vertIndex].mPositionVertex = PositionVertex(-halfX, -halfY, -halfZ);
		vertices[vertIndex].mNormalVertex = NormalVertex(-1.0f, 0.0f, 0.0);
		vertices[vertIndex++].mUVVertex = UVVertex(1.0f, 0.0f);
		vertices[vertIndex].mPositionVertex = PositionVertex(-halfX, halfY, halfZ);
		vertices[vertIndex].mNormalVertex = NormalVertex(-1.0f, 0.0f, 0.0);
		vertices[vertIndex++].mUVVertex = UVVertex(0.0f, 1.0f);
		vertices[vertIndex].mPositionVertex = PositionVertex(-halfX, halfY, -halfZ);
		vertices[vertIndex].mNormalVertex = NormalVertex(-1.0f, 0.0f, 0.0);
		vertices[vertIndex++].mUVVertex = UVVertex(1.0f, 1.0f);
		
		// Indices
		const unsigned int indicesCount = 36;
		IndexData* indices = new IndexData[indicesCount];
		
		for (int i = 0; i < 6; ++i)
		{
			const int offset = i * 4;
			indices[i * 6] = offset;
			indices[(i * 6) + 1] = offset + 2;
			indices[(i * 6) + 2] = offset + 1;
			indices[(i * 6) + 3] = offset + 3;
			indices[(i * 6) + 4] = offset + 1;
			indices[(i * 6) + 5] = offset + 2;
		}
		
		Mesh mesh (DrawMode::TRIANGLES, vertices, verticesCount, indices, indicesCount);
		mesh.Setup();
		return mesh;
	}
	
	Mesh MeshUtils::CreateSpike(
		float sizeX /* = 1.0f */,
		float sizeY /* = 1.0f */,
		float sizeZ /* = 1.0f */
	) {
		const float halfX = sizeX*0.5f;
		const float halfY = sizeY*0.5f;
		const float halfZ = sizeZ*0.5f;
		
		const unsigned int verticesCount = 4 + 4 * 3;
		VertexData* vertices = new VertexData[verticesCount];
		
		int vertIndex = 0;
		
		// Bottom
		vertices[vertIndex].mPositionVertex = PositionVertex(-halfX, -halfY, halfZ);
		vertices[vertIndex++].mNormalVertex = NormalVertex(0.0f);
		vertices[vertIndex].mPositionVertex = PositionVertex(halfX, -halfY, halfZ);
		vertices[vertIndex++].mNormalVertex = NormalVertex(0.0f);
		vertices[vertIndex].mPositionVertex = PositionVertex(-halfX, -halfY, -halfZ);
		vertices[vertIndex++].mNormalVertex = NormalVertex(0.0f);
		vertices[vertIndex].mPositionVertex = PositionVertex(halfX, -halfY, -halfZ);
		vertices[vertIndex++].mNormalVertex = NormalVertex(0.0f);
		
		// Front
		vertices[vertIndex].mPositionVertex = PositionVertex(-halfX, -halfY, -halfZ);
		vertices[vertIndex++].mNormalVertex = NormalVertex(0.0f);
		vertices[vertIndex].mPositionVertex = PositionVertex(halfX, -halfY, -halfZ);
		vertices[vertIndex++].mNormalVertex = NormalVertex(0.0f);
		vertices[vertIndex].mPositionVertex = PositionVertex(0.0f, halfY, 0.0f);
		vertices[vertIndex++].mNormalVertex = NormalVertex(0.0f);
		
		// Right
		vertices[vertIndex].mPositionVertex = PositionVertex(halfX, -halfY, -halfZ);
		vertices[vertIndex++].mNormalVertex = NormalVertex(0.0f);
		vertices[vertIndex].mPositionVertex = PositionVertex(halfX, -halfY, halfZ);
		vertices[vertIndex++].mNormalVertex = NormalVertex(0.0f);
		vertices[vertIndex].mPositionVertex = PositionVertex(0.0f, halfY, 0.0f);
		vertices[vertIndex++].mNormalVertex = NormalVertex(0.0f);
		
		// Back
		vertices[vertIndex].mPositionVertex = PositionVertex(halfX, -halfY, halfZ);
		vertices[vertIndex++].mNormalVertex = NormalVertex(0.0f);
		vertices[vertIndex].mPositionVertex = PositionVertex(-halfX, -halfY, halfZ);
		vertices[vertIndex++].mNormalVertex = NormalVertex(0.0f);
		vertices[vertIndex].mPositionVertex = PositionVertex(0.0f, halfY, 0.0f);
		vertices[vertIndex++].mNormalVertex = NormalVertex(0.0f);
		
		// Left
		vertices[vertIndex].mPositionVertex = PositionVertex(-halfX, -halfY, halfZ);
		vertices[vertIndex++].mNormalVertex = NormalVertex(0.0f);
		vertices[vertIndex].mPositionVertex = PositionVertex(-halfX, -halfY, -halfZ);
		vertices[vertIndex++].mNormalVertex = NormalVertex(0.0f);
		vertices[vertIndex].mPositionVertex = PositionVertex(0.0f, halfY, 0.0f);
		vertices[vertIndex++].mNormalVertex = NormalVertex(0.0f);
		
		// Indices
		const unsigned int indicesCount = 18;
		IndexData* indices = new IndexData[indicesCount];
		
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;
		indices[3] = 2;
		indices[4] = 1;
		indices[5] = 3;
		indices[6] = 4;
		indices[7] = 5;
		indices[8] = 6;
		indices[9] = 7;
		indices[10] = 8;
		indices[11] = 9;
		indices[12] = 10;
		indices[13] = 11;
		indices[14] = 12;
		indices[15] = 13;
		indices[16] = 14;
		indices[17] = 15;
		
		Mesh mesh (DrawMode::TRIANGLES, vertices, verticesCount, indices, indicesCount);
		mesh.Setup();
		return mesh;
	}
	
	Mesh MeshUtils::CreateSphere(
		float radius /* = 1.0f */,
		int sectors /* = 10 */,
		int rings /* = 10 */
	) {
		const unsigned int verticesCount = (sectors + 1) * (rings + 1);
		VertexData* vertices = new VertexData[verticesCount];
		
		const unsigned int indicesCount = sectors * rings * 6 - sectors * 6;
		IndexData* indices = new IndexData[indicesCount];
		
		const float sectorStep = static_cast<float>(2.0f * M_PI / sectors);
		const float stackStep = static_cast<float>(M_PI / rings);
		float stackAngle;
		int index = 0;
		
		for (int i = 0; i <= rings; ++i)
		{
			stackAngle = static_cast<float>(M_PI / 2.0f - i * stackStep);
			const float xy = radius * cosf(stackAngle);
			const float z = radius * sinf(stackAngle);
			
			for (int j = 0; j <= sectors; ++j)
			{
				const float sectorAngle = j * sectorStep;
				const float x = xy * cosf(sectorAngle);
				const float y = xy * sinf(sectorAngle);
				vertices[index].mPositionVertex = PositionVertex(x, y, z);
				vertices[index].mNormalVertex = NormalVertex(0.0f);
				vertices[index].mUVVertex = UVVertex(static_cast<float>(j / sectors), static_cast<float>(i / rings));
				
				++index;
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
					indices[index++] = k1;
					indices[index++] = k2;
					indices[index++] = k1 + 1;
				}
				
				if (i != (rings-1))
				{
					indices[index++] = k1 + 1;
					indices[index++] = k2;
					indices[index++] = k2 + 1;
				}
			}
		}
		
		Mesh mesh (DrawMode::TRIANGLES, vertices, verticesCount, indices, indicesCount);
		mesh.Setup();
		return mesh;
	}
	
	Mesh MeshUtils::CreateGrid(
		int sizeX /*= 100*/,
		int sizeY /*= 100*/,
		float width /*= 1.0f*/,
		float height /*= 1.0f*/
	)
	{
		const int halfSizeX = sizeX / 2;
		const int halfSizeY = sizeY / 2;
		
		const unsigned int verticesCount = (sizeX + 1) * 2 + (sizeY + 1) * 2;
		VertexData* vertices = new VertexData[verticesCount];
		
		int index = 0;
		
		for (int i = 0; i < sizeX + 1; ++i)
		{
			vertices[index].mPositionVertex = PositionVertex(- halfSizeX * width, 0.0f, height * (i - halfSizeY));
			vertices[index].mNormalVertex = NormalVertex(0.0f, 0.0f, 0.0f);
			++index;
		}
		
		for (int i = 0; i < sizeX + 1; ++i)
		{
			vertices[index].mPositionVertex = PositionVertex(halfSizeX * width, 0.0f, height * (i - halfSizeY));
			vertices[index].mNormalVertex = NormalVertex(0.0f, 0.0f, 0.0f);
			++index;
		}
		
		for (int i = 0; i < sizeY + 1; ++i)
		{
			vertices[index].mPositionVertex = PositionVertex(width * (i - halfSizeX), 0.0f, - halfSizeY * height);
			vertices[index].mNormalVertex = NormalVertex(0.0f, 0.0f, 0.0f);
			++index;
		}
		
		for (int i = 0; i < sizeY + 1; ++i)
		{
			vertices[index].mPositionVertex = PositionVertex(width * (i - halfSizeX), 0.0f, halfSizeY * height);
			vertices[index].mNormalVertex = NormalVertex(0.0f, 0.0f, 0.0f);
			++index;
		}
		
		const unsigned int indicesCount = verticesCount;
		IndexData* indices = new IndexData[indicesCount];
		
		index = 0;
		
		for (int i = 0; i < sizeX + 1; ++i)
		{
			indices[index * 2] = i;
			indices[(index * 2) + 1] = i + (sizeX + 1);
			++index;
		}
		
		for (int i = 0; i < sizeY + 1; ++i)
		{
			const int start = i + (sizeX + 1) * 2;
			indices[index * 2] = start;
			indices[index * 2 + 1] = start + (sizeY + 1);
			++index;
		}
		
		Mesh mesh (DrawMode::LINES, vertices, verticesCount, indices, indicesCount);
		mesh.Setup();
		return mesh;
	}
	
}
