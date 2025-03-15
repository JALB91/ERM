#pragma once

namespace erm {
class Engine;
struct Mesh;
} // namespace erm

namespace ImGui {

extern void ShowMeshDebugWindow(erm::Engine& engine, erm::Mesh& mesh, unsigned int meshId);

}
