#pragma once

#include <memory>

namespace erm {
	
	struct GameConfig;
	class Renderer;
	class IModelLoader;
	class ISceneObject;
	
	class Game
	{
	public:
		Game(GameConfig& gameConfig);
		~Game();
		
		void Init();
		
		void OnMouseButtonPressed();
		void OnMouseButtonReleased();
		void OnMousePos(double xPos, double yPos);
		
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
		
		double mMousePosX, mMousePosY;
		bool mIsDragging;
		
	};
	
}
