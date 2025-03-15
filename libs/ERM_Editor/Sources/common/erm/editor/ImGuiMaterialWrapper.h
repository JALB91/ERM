#pragma once

namespace erm {
class Engine;
struct Mesh;
} // namespace erm

namespace ImGui {

extern void ShowMaterialDebug(erm::Engine& engine, erm::Mesh& mesh);

}
