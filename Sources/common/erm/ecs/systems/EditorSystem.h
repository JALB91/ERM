#pragma once

#include "erm/ecs/ISystem.h"
#include "erm/ecs/components/EditorComponent.h"

#include "erm/ecs/EntityId.h"

#include "erm/rendering/buffers/DeviceBuffer.h"

#include "erm/rendering/data_structs/Model.h"
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
class TransformSystem;
class SkeletonSystem;
class ModelSystem;
class CameraSystem;
class LightSystem;
class RenderingSystem;
struct Entity;
} // namespace ecs
} // namespace erm

namespace erm::ecs {

class EditorSystem : public ISystem
{
	ERM_SYSTEM_DECL(Editor)

public:
	EditorSystem(Engine& engine);
	~EditorSystem();

	// ISystem
	void Init() override;
	void OnRender() override;

private:
	// ISystem
	void OnComponentAdded(EntityId id) override;
	void OnComponentBeingRemoved(EntityId id) override;

	RenderData& GetOrCreateRenderDataForBBox(EntityId id);

	Device& mDevice;
	Renderer& mRenderer;
	ResourcesManager& mResourcesManager;

	TransformSystem* mTransformSystem;
	SkeletonSystem* mSkeletonSystem;
	ModelSystem* mModelSystem;
	CameraSystem* mCameraSystem;
	LightSystem* mLightSystem;
	RenderingSystem* mRenderingSystem;

#ifdef ERM_RAY_TRACING_ENABLED
	Model mPlaneModel;
	DeviceBuffer mInstanceDataBuffer;
#endif

	RenderData mGridRenderData;
	StandaloneMesh mGridMesh;

	PipelineConfigs mBBoxPipelineConfigs;
	std::map<EntityId, std::pair<RenderData, StandaloneMesh>> mBBoxesRenderData;

	RenderData mArrowsRenderData;
	std::vector<StandaloneMesh> mArrowsMeshes;

	PipelineConfigs mBonesPipelineConfigs;
	std::map<EntityId, std::vector<StandaloneMesh>> mBonesMeshes;
};

} // namespace erm::ecs
