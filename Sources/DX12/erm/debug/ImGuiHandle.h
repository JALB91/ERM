#pragma once

namespace erm {
	class Engine;
}

namespace erm {

	class ImGuiHandle
	{
	public:
		ImGuiHandle(Engine& engine);

		void OnUpdate();
		void OnPreRender();
		void OnRender();
		void OnPostRender();

	private:
		Engine& mEngine;
	};

} // namespace erm