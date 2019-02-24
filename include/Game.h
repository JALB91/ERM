#pragma once

#include "ISceneObject.h"
#include "IModelLoader.h"

#include <memory>

namespace erm {
	
	struct GameConfig;
	class Renderer;
	
	class Game
	{
	public:
		Game(GameConfig& gameConfig);
		
		void Init();
		
		void OnSizeChanged(int width, int height);
		void OnUpdate(float dt);
		void OnImGuiRender();
		void OnRender();
		
		inline const Renderer* const GetRenderer() const { return mRenderer.get(); }
		inline const IModelLoader* const GetModelParser() const { return mModelLoader.get(); }
		
	private:
		GameConfig& mGameConfig;
		std::unique_ptr<Renderer> mRenderer;
		std::unique_ptr<IModelLoader> mModelLoader;
		
		std::unique_ptr<ISceneObject> mRoot;
		
	};
	
}
