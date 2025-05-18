#pragma once

namespace erm {
struct Mesh;
} // namespace erm

namespace ImGui {

extern void ShowMeshDebugWindow(erm::Mesh& mesh, unsigned int meshId);

}
