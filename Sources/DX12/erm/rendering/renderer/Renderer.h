#pragma once

namespace erm {
	class Engine;
}

namespace erm {

	class Renderer
	{
	public:
		Renderer(Engine& engine);

		void OnPreRender();
		void OnRender();
		void OnPostRender();

	private:
		Engine& mEngine;
	};

} // namespace erm