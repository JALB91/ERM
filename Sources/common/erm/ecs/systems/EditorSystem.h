#pragma once

#include "erm/ecs/ISystem.h"
#include "erm/ecs/components/EditorComponent.h"

#include "erm/ecs/EntityId.h"

#include "erm/rendering/data_structs/RenderData.h"
#include "erm/rendering/data_structs/StandaloneMesh.h"

#include <map>
#include <vector>

namespace erm {
class Device;
class Engine;
class Renderer;
class ShaderProgram;
class ResourcesManager;
namespace ecs {
struct Entity;
} // namespace ecs
} // namespace erm

namespace erm::ecs {

class EditorSystem : public ISystem
{
	ERM_SYSTEM_DECL(Editor)

public:
	EditorSystem(Engine& engine);

	// ISystem
	void OnPreRender() override;

private:
	// ISystem
	void OnComponentBeingRemoved(EntityId id) override;

	RenderData& GetOrCreateRenderDataForBBox(EntityId id);

	Device& mDevice;
	Renderer& mRenderer;
	ResourcesManager& mResourcesManager;

	RenderData mGridRenderData;
	StandaloneMesh mGridMesh;

	PipelineConfigs mBBoxPipelineConfigs;
	std::map<EntityId, std::pair<RenderData, StandaloneMesh>> mBBoxesRenderData;

	PipelineConfigs mBonesPipelineConfigs;
	std::map<EntityId, std::vector<StandaloneMesh>> mBonesMeshes;
};

} // namespace erm::ecs
