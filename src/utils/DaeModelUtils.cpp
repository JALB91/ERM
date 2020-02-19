#include "utils/DaeModelUtils.h"
#include "utils/Utils.h"

#include "rendering/VertexData.h"
#include "rendering/IndexData.h"

#include <tinyxml2.h>

#include <GLFW/glfw3.h>

#include <array>
#include <iostream>
#include <map>

using namespace tinyxml2;

namespace erm {
	
	void ReadSources(
		XMLElement* mesh,
		std::vector<Vertex>& pVertices,
		std::vector<UVVertex>& uvVertices,
		std::vector<NormalVertex>& nVertices
	);
	
	void ParseDaeModel(
		std::mutex& mutex,
		std::atomic<bool>& stop,
		const char* path,
		Model& model,
		Materials& materials
	)
	{
		(void)materials;
		
		XMLDocument document;
		XMLError error = document.LoadFile(path);
		
		if (error != XMLError::XML_SUCCESS)
		{
			std::cout << "Error %d while reading dae file %s" << error << path << std::endl;
			return;
		}

		XMLElement* geometry =
		  document.RootElement()->FirstChildElement("library_geometries")->FirstChildElement("geometry");

		while (geometry)
		{
			if (stop) return;
			
			const std::string name = geometry->Attribute("id");
			
			XMLElement* mesh = geometry->FirstChildElement("mesh");
			
			while (mesh)
			{
				XMLElement* primitive = mesh->FirstChildElement("polylist");
				
				if (!primitive)
				{
					mesh = mesh->NextSiblingElement("mesh");
					continue;
				}
				
				bool isValid = true;
				
				for (const std::string value : Utils::SplitString(primitive->FirstChildElement("vcount")->GetText(), ' '))
				{
					isValid &= (std::atoi(value.c_str()) == 3);
				}
				
				if (!isValid)
				{
					mesh = mesh->NextSiblingElement("mesh");
					continue;
				}
				
				std::vector<Vertex> pVertices;
				std::vector<UVVertex> uvVertices;
				std::vector<NormalVertex> nVertices;
				
				ReadSources(mesh, pVertices, uvVertices, nVertices);
				
				int primCount;
				primitive->QueryIntAttribute("count", &primCount);
				const int indicesCount = primCount * 3;
				
				int offset = 1;
				if (!nVertices.empty()) offset++;
				if (!uvVertices.empty()) offset++;
				
				const std::vector<std::string> values = Utils::SplitString(primitive->FirstChildElement("p")->GetText(), ' ');
				
				IndexData* indicesData = new IndexData[indicesCount];
				VertexData* verticesData = new VertexData[indicesCount];
				
				for (unsigned int i = 0; i < static_cast<unsigned int>(values.size()); i += offset)
				{
					const int index = i / offset;
					VertexData vertexData;
					
					indicesData[index] = index;
					
					const unsigned short pIndex = std::atoi(values[i].c_str());
					vertexData.mVertex = pVertices[pIndex];
					
					if (!nVertices.empty())
					{
						const unsigned short nIndex = std::atoi(values[i+1].c_str());
						vertexData.mNormalVertex = nVertices[nIndex];
					}
					
					if (!uvVertices.empty())
					{
						const unsigned int uvOffset = nVertices.empty() ? 1 : 2;
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

	void ReadSources(
		XMLElement* mesh,
		std::vector<Vertex>& pVertices,
		std::vector<UVVertex>& uvVertices,
		std::vector<NormalVertex>& nVertices
	)
	{
		std::string pSourceStr = mesh->FirstChildElement("vertices")->FirstChildElement("input")->Attribute("source");
		pSourceStr = pSourceStr.erase(0, 1);
		XMLElement* pSource = mesh->FirstChildElement("source");
		while (pSource)
		{
			if (pSource->Attribute("id") == pSourceStr) break;
			pSource = pSource->NextSiblingElement("source");
		}
		std::string pSourceVecStr = pSource->FirstChildElement("float_array")->GetText();
		std::vector<std::string> pSourceVec (Utils::SplitString(pSourceVecStr, ' '));
		
		ASSERT(pSourceVec.size() % 3 == 0);
		
		for (unsigned int i = 0; i < static_cast<unsigned int>(pSourceVec.size()); i += 3)
		{
			pVertices.emplace_back(
				atof(pSourceVec[i].c_str()),
				atof(pSourceVec[i+1].c_str()),
				atof(pSourceVec[i+2].c_str())
			);
		}
		
		XMLElement* firstInput = mesh->FirstChildElement("polylist")->FirstChildElement("input")->NextSiblingElement("input");
		const std::string firstInputSemantic = firstInput->Attribute("semantic");
		
		if (!firstInput) return;
		
		XMLElement* secondInput = firstInput->NextSiblingElement("input");
		
		XMLElement* uvInput = nullptr;
		XMLElement* nInput = nullptr;
		
		
		if (firstInputSemantic == "TEXCOORD") uvInput = firstInput;
		else if (firstInputSemantic == "NORMAL") nInput = firstInput;
		
		if (secondInput)
		{
			const std::string secondInputSemantic = secondInput->Attribute("semantic");
			if (secondInputSemantic == "TEXCOORD") uvInput = secondInput;
			else if (secondInputSemantic == "NORMAL") nInput = secondInput;
		}
		
		
		if (uvInput)
		{
			std::string uvSourceStr = uvInput->Attribute("source");
			uvSourceStr = uvSourceStr.erase(0, 1);
			XMLElement* uvSource = mesh->FirstChildElement("source");
			while (uvSource)
			{
				if (uvSource->Attribute("id") == uvSourceStr) break;
				uvSource = uvSource->NextSiblingElement("source");
			}
			if (uvSource)
			{
				std::string uvSourceVecStr = uvSource->FirstChildElement("float_array")->GetText();
				std::vector<std::string> uvSourceVec (Utils::SplitString(uvSourceVecStr, ' '));
				
				ASSERT(uvSourceVec.size() % 2 == 0);
				
				for (unsigned int i = 0; i < static_cast<unsigned int>(uvSourceVec.size()); i += 2)
				{
					uvVertices.emplace_back(
						atof(uvSourceVec[i].c_str()),
						atof(uvSourceVec[i+1].c_str())
					);
				}
			}
		}
		
		if (nInput)
		{
			std::string nSourceStr = nInput->Attribute("source");
			nSourceStr = nSourceStr.erase(0, 1);
			XMLElement* nSource = mesh->FirstChildElement("source");
			while (nSource)
			{
				if (nSource->Attribute("id") == nSourceStr) break;
				nSource = nSource->NextSiblingElement("source");
			}
			if (nSource)
			{
				std::string nSourceVecStr = nSource->FirstChildElement("float_array")->GetText();
				std::vector<std::string> nSourceVec (Utils::SplitString(nSourceVecStr, ' '));
				
				ASSERT(nSourceVec.size() % 3 == 0);
				
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
	}
	
}
