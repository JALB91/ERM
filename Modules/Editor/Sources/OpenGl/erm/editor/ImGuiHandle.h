#pragma once

namespace erm {
class Engine;
} // namespace erm

namespace erm {

class ImGuiHandle
{
public:
	ImGuiHandle(Engine& engine);
	~ImGuiHandle();

	void OnUpdate();

	void OnPreRender();
	void OnRender();
	void OnPostRender();

private:
	Engine& mEngine;
};

} // namespace erm
