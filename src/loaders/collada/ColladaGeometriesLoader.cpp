#include "loaders/collada/ColladaGeometriesLoader.h"

#include "loaders/collada/ColladaLoaderUtils.h"

#include "rendering/buffers/IndexData.h"
#include "rendering/data_structs/Mesh.h"
#include "rendering/data_structs/Model.h"

#include "utils/Utils.h"

#include <tinyxml2.h>

#include <array>
#include <string>

using namespace tinyxml2;

namespace {
	std::array<const char*, 2> kSupportedPrimitives {
		"polylist",
		"vertices"
	};
}

namespace erm {
	
	void ReadVerticesSources(
		XMLElement& mesh,
		const char* primitive,
		std::vector<PositionVertex>& pVertices,
		std::vector<NormalVertex>& nVertices,
		std::vector<UVVertex>& uvVertices
	);
	
	void ProcessGeometries(
		std::mutex& mutex,
		std::atomic<bool>& stop,
		XMLDocument& document,
		const std::vector<BoneIds>& boneIds,
		const std::vector<BoneWeights>& boneWeights,
		Model& model
	)
	{
		XMLElement* geometry =
		  document.RootElement()->FirstChildElement("library_geometries")->FirstChildElement("geometry");

		while (geometry)
		{
			if (stop) return;
			
			const char* name = geometry->Attribute("id");
			
			XMLElement* mesh = geometry->FirstChildElement("mesh");
			
			while (mesh)
			{
				XMLElement* primitive = nullptr;
				
				for (const char* primitiveName : kSupportedPrimitives)
				{
					if ((primitive = mesh->FirstChildElement(primitiveName))) break;
				}
				
				bool isValid = true;
				
				if (primitive)
				{
					for (const std::string& value : Utils::SplitString(primitive->FirstChildElement("vcount")->GetText(), ' '))
					{
						isValid &= (std::atoi(value.c_str()) == 3);
					}
				}
				
				if (!isValid)
				{
					mesh = mesh->NextSiblingElement("mesh");
					continue;
				}
				
				int primCount;
				primitive->QueryIntAttribute("count", &primCount);
				const int indicesCount = primCount * 3;
				
				std::vector<PositionVertex> pVertices;
				std::vector<NormalVertex> nVertices;
				std::vector<UVVertex> uvVertices;
				
				ReadVerticesSources(*mesh, primitive->Name(), pVertices, nVertices, uvVertices);
				
				int totalOffset = 0;
				int pOffset = -1;
				int nOffset = -1;
				int uvOffset = -1;
				
				XMLElement* input = primitive->FirstChildElement("input");
				
				while (input)
				{
					int offset;
					input->QueryIntAttribute("offset", &offset);
					
					if (std::strcmp(input->Attribute("semantic"), "VERTEX") == 0)
					{
						pOffset = offset;
					}
					else if (std::strcmp(input->Attribute("semantic"), "NORMAL") == 0)
					{
						nOffset = offset;
					}
					else if (std::strcmp(input->Attribute("semantic"), "TEXCOORD") == 0)
					{
						uvOffset = offset;
					}
					
					totalOffset = std::max(totalOffset, offset);
					
					input = input->NextSiblingElement("input");
				}
				
				++totalOffset;
				
				const std::vector<std::string> values = Utils::SplitString(primitive->FirstChildElement("p")->GetText(), ' ');
				
				IndexData* indicesData = new IndexData[indicesCount];
				VertexData* verticesData = new VertexData[indicesCount];
				
				for (unsigned int i = 0; i < static_cast<unsigned int>(values.size()); i += totalOffset)
				{
					const unsigned int index = i / totalOffset;
					VertexData vertexData;
					
					indicesData[index] = index;
					
					const unsigned short pIndex = std::atoi(values[i + pOffset].c_str());
					vertexData.mPositionVertex = pVertices[pIndex];
					vertexData.mBoneIds = boneIds[pIndex];
					vertexData.mBoneWeights = boneWeights[pIndex];
					
					if (!nVertices.empty() && nOffset >= 0)
					{
						const unsigned short nIndex = std::atoi(values[i+nOffset].c_str());
						vertexData.mNormalVertex = nVertices[nIndex];
					}
					
					if (!uvVertices.empty() && uvOffset >= 0)
					{
						const unsigned short uvIndex = std::atoi(values[i+uvOffset].c_str());
						vertexData.mUVVertex = uvVertices[uvIndex];
					}
					
					verticesData[index] = vertexData;
				}
				
				mutex.lock();
				model.AddMesh(Mesh(DrawMode::TRIANGLES, verticesData, indicesCount, indicesData, indicesCount, nullptr, name));
				mutex.unlock();
				
				mesh = mesh->NextSiblingElement("mesh");
			}
			geometry = geometry->NextSiblingElement("geometry");
		}
	}
	
	void ReadVerticesSources(
		XMLElement& mesh,
		const char* primitive,
		std::vector<PositionVertex>& pVertices,
		std::vector<NormalVertex>& nVertices,
		std::vector<UVVertex>& uvVertices
	)
	{
		std::string pSourceStr = mesh.FirstChildElement("vertices")->FirstChildElement("input")->Attribute("source");
		pSourceStr = pSourceStr.substr(1);
		
		XMLElement* pSource = FindChildWithAttribute(mesh, "source", "id", pSourceStr.c_str());
		const std::vector<std::string> pSourceVec (Utils::SplitString(pSource->FirstChildElement("float_array")->GetText(), ' '));
		
		ASSERT(pSourceVec.size() % 3 == 0);
		
		pVertices.reserve(pSourceVec.size() / 3);
		
		for (unsigned int i = 0; i < static_cast<unsigned int>(pSourceVec.size()); i += 3)
		{
			pVertices.emplace_back(
				atof(pSourceVec[i].c_str()),
				atof(pSourceVec[i+1].c_str()),
				atof(pSourceVec[i+2].c_str())
			);
		}
		
		XMLElement* primitiveElement = mesh.FirstChildElement(primitive);
		XMLElement* uvInput = FindChildWithAttribute(*primitiveElement, "input", "semantic", "TEXCOORD");
		XMLElement* nInput = FindChildWithAttribute(*primitiveElement, "input", "semantic", "NORMAL");
		
		if (nInput)
		{
			std::string nSourceStr = nInput->Attribute("source");
			nSourceStr = nSourceStr.erase(0, 1);
			XMLElement* nSource = FindChildWithAttribute(mesh, "source", "id", nSourceStr.c_str());
			
			if (nSource)
			{
				std::vector<std::string> nSourceVec (Utils::SplitString(nSource->FirstChildElement("float_array")->GetText(), ' '));
				
				ASSERT(nSourceVec.size() % 3 == 0);
				
				nVertices.reserve(nSourceVec.size() / 3);
				
				for (unsigned int i = 0; i < static_cast<unsigned int>(nSourceVec.size()); i += 3)
				{
					nVertices.emplace_back(
						atof(nSourceVec[i].c_str()),
						atof(nSourceVec[i+1].c_str()),
						atof(nSourceVec[i+2].c_str())
					);
				}
			}
		}
		
		if (uvInput)
		{
			std::string uvSourceStr = uvInput->Attribute("source");
			uvSourceStr = uvSourceStr.erase(0, 1);
			XMLElement* uvSource = FindChildWithAttribute(mesh, "source", "id", uvSourceStr.c_str());
			
			if (uvSource)
			{
				std::vector<std::string> uvSourceVec (Utils::SplitString(uvSource->FirstChildElement("float_array")->GetText(), ' '));
				
				ASSERT(uvSourceVec.size() % 2 == 0);
				
				uvVertices.reserve(uvSourceVec.size() / 3);
				
				for (unsigned int i = 0; i < static_cast<unsigned int>(uvSourceVec.size()); i += 2)
				{
					uvVertices.emplace_back(
						atof(uvSourceVec[i].c_str()),
						atof(uvSourceVec[i+1].c_str())
					);
				}
			}
		}
	}
	
}
