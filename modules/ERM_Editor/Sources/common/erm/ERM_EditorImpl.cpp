#include "erm/ERM_EditorImpl.h"

#include "erm/ERM_Editor.h"
#include "erm/editor/ImGuiHandle.h"

#include <erm/engine/Engine.h>
#include <erm/modules_lib/ObjectRegistry.h>

#include <cstdlib>

namespace erm {

ERM_Editor::ERM_Editor() noexcept = default;

int ERM_Editor::run(int /*argc*/, char** /*argv*/)
{
	auto& engine = ObjectRegistry::require<Engine>();

	erm::ImGuiHandle handle;

	engine.run();

	return EXIT_SUCCESS;
}

}
