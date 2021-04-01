#pragma once

namespace erm {
class Engine;
class Mesh;
} // namespace erm

namespace ImGui {

extern void ShowPBMaterialDebug(erm::Engine& engine, erm::Mesh& mesh);

}
