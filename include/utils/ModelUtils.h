#pragma once

namespace erm {
	
	class Model;
	
	class ModelUtils
	{
	public:
		static Model CreateCube(
			float sizeX = 2.0f,
			float sizeY = 2.0f,
			float sizeZ = 2.0f
		);
		static Model CreateSpike(
			float sizeX = 2.0f,
			float sizeY = 2.0f,
			float sizeZ = 2.0f
		);
		static Model CreateSphere(
			float radius = 1.0f,
			int stackCount = 10,
			int sectorCount = 10
		);
		
	};
	
}
