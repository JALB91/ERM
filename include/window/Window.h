#pragma once

#include "interfaces/IKeyInfoProvider.h"
#include "interfaces/IMouseInfoProvider.h"
#include "interfaces/IWindowSizeProvider.h"

#include <glm/glm.hpp>

#include <set>

struct GLFWwindow;

namespace erm {
	
	class Window:
		public IKeyInfoProvider,
		public IMouseInfoProvider,
		public IWindowSizeProvider
	{
	public:
		Window(int width, int height);
		~Window();
		
		bool Init();
		bool ShouldClose();
		void NewFrame();
		void Render();
		void PostRender();
		
		bool IsDepthEnabled() const;
		int GetDepthFunction() const;
		void SetDepthEnabled(bool enabled) const;
		void SetDepthFunction(int depthFunc) const;
		
		bool IsBlendEnabled() const;
		int GetBlendSourceFactor() const;
		int GetBlendDestinationFactor() const;
		void SetBlendEnabled(bool enabled) const;
		void SetBlendFunction(int sFactor, int dFactor) const;
		
		bool IsCullFaceEnabled() const;
		int GetCullFace() const;
		int GetCullFrontFace() const;
		void SetCullFaceEnabled(bool enabled) const;
		void SetCullFace(int cullFace) const;
		void SetFrontFace(int frontFace) const;
		
		int GetPolygonMode() const;
		void SetPolygonMode(int mode) const;
		
		glm::vec4 GetClearColor() const;
		void SetClearColor(const glm::vec4& clearColor) const;
		
		void OnKey(int key, int scanCode, int action, int mods);
		void OnMouseButton(int button, int action, int mods);
		void OnMousePos(double xPos, double yPos);
		void OnSizeChanged(int width, int height);
		void OnSizeChanged();
		
		// IKeyInfoProvider
		void AddListener(IKeyListener& listener) override;
		void RemoveListener(IKeyListener& listener) override;
		bool IsKeyDown(int keyCode) const override;
		
		// IMouseInfoProvider
		void AddListener(IMouseListener& listener) override;
		void RemoveListener(IMouseListener& listener) override;
		inline double GetMousePosX() const override { return mMousePosX; }
		inline double GetMousePosY() const override { return mMousePosY; }
		inline double GetPreviousMousePosX() const override { return mPrevMousePosX; }
		inline double GetPreviousMousePosY() const override { return mPrevMousePosY; }
		bool IsMouseButtonDown(int mouseButton) const override;
		
		// IWindowSizeProvider
		void AddListener(IWindowSizeListener& listener) override;
		void RemoveListener(IWindowSizeListener& listener) override;
		inline int GetWindowWidth() const override { return mWindowWidth; }
		inline int GetWindowHeight() const override { return mWindowHeight; }
		inline float GetAspectRatio() const override { return mAspectRatio; }
		
	private:
		void UpdateViewport();
		void UpdateAspectRatio();

		GLFWwindow* mWindow;
		
		std::set<IKeyListener*> mKeyListeners;
		std::set<IMouseListener*> mMouseListeners;
		std::set<IWindowSizeListener*> mWindowSizeListeners;
		
		std::set<int> mPressedKeys;
		double mMousePosX, mPrevMousePosX;
		double mMousePosY, mPrevMousePosY;
		std::set<int> mPressedButtons;
		int mWindowWidth, mWindowHeight;
		float mAspectRatio;
		
	};
	
}