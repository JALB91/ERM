#include "erm/rendering/ray_tracing/RTRenderingResources.h"

#include <erm/math/math.h>
#include <erm/math/BoundingBox.h>

#include "erm/rendering/ray_tracing/RTBlas.h"
#include "erm/rendering/ray_tracing/RTPipelineResources.h"
#include "erm/rendering/ray_tracing/RTRenderData.h"
#include "erm/rendering/Device.h"
#include "erm/rendering/buffers/DeviceBuffer.h"
#include "erm/rendering/renderer/Renderer.h"
#include "erm/rendering/shaders/IShaderProgram.h"
#include "erm/rendering/utils/VkUtils.h"

#include <erm/window/Window.h>

#include <erm/utils/Profiler.h>

#include <algorithm>

namespace erm {

RTRenderingResources::RTRenderingResources(Device& device, IRenderer& renderer)
	: mDevice(device)
	, mRenderer(renderer)
{
	createDescriptorPool();
}

RTRenderingResources::~RTRenderingResources() = default;

void RTRenderingResources::refresh()
{
	if (mPipelineResources)
	{
		mPipelineResources->refresh();
	}
}

void RTRenderingResources::updateCommandBuffer(vk::CommandBuffer& cmd, RTRenderData& renderData)
{
	ERM_PROFILE_FUNCTION();

	ERM_ASSERT(!renderData.mInstancesMap.empty());

	updateResources(renderData);

	const vk::Extent2D& extent = mRenderer.getSwapChainExtent();

	mPipelineResources->updateCommandBuffer(cmd, renderData);

	const vk::PhysicalDeviceRayTracingPipelinePropertiesKHR& rtProps = mDevice.getRayTracingProperties();

	// Size of a program identifier
	u32 groupSize = math::alignUp(rtProps.shaderGroupHandleSize, rtProps.shaderGroupBaseAlignment);
	u32 groupStride = groupSize;
	vk::DeviceAddress sbtAddress = mDevice->getBufferAddress({mPipelineResources->getSBTBuffer()});

	// TODO: DAMIO
	//const auto& shader = *renderData.mPipelineConfigs.mShaderProgram;
	//const auto& shadersData = shader.getShadersDataMap();
	ShadersDataMap shadersData;
	const u64 genNum = shadersData.at(ShaderType::RT_RAY_GEN).size();
	const u64 missNum = shadersData.at(ShaderType::RT_MISS).size();
	const u64 chHitNum = shadersData.at(ShaderType::RT_CLOSEST_HIT).size();

	using Stride = vk::StridedDeviceAddressRegionKHR;
	std::array<Stride, 4> strideAddresses {
		Stride {sbtAddress + 0u * groupSize, groupStride, groupSize * genNum},
		Stride {sbtAddress + genNum * groupSize, groupStride, groupSize * missNum},
		Stride {sbtAddress + (genNum + missNum) * groupSize, groupStride, groupSize * chHitNum},
		Stride {0u, 0u, 0u}}; // callable

	cmd.traceRaysKHR(
		&strideAddresses[0],
		&strideAddresses[1],
		&strideAddresses[2],
		&strideAddresses[3],
		extent.width,
		extent.height,
		1);
}

void RTRenderingResources::updateResources(RTRenderData& renderData)
{
	ERM_PROFILE_FUNCTION();

	const bool forceUpdate = renderData.mForceUpdate || (mPipelineResources && mPipelineResources->getMaxInstancesCount() != renderData.mInstancesMap.size());

	if (forceUpdate)
	{
		mTopLevelAS.reset();
	}

	buildBlas(renderData, vk::BuildAccelerationStructureFlagBitsKHR::ePreferFastTrace);
	updateTopLevelAS(renderData, vk::BuildAccelerationStructureFlagBitsKHR::ePreferFastTrace | vk::BuildAccelerationStructureFlagBitsKHR::eAllowUpdate);

	if (!mPipelineResources || forceUpdate)
	{
		mPipelineResources = std::make_unique<RTPipelineResources>(
			mDevice,
			mRenderer,
			renderData,
			mDescriptorPool.get(),
			&mTopLevelAS.getAS());
	}
}

void RTRenderingResources::buildBlas(RTRenderData& data, vk::BuildAccelerationStructureFlagsKHR flags)
{
	ERM_PROFILE_FUNCTION();

	std::vector<RTBlas*> toBuild;
	for (const auto& [id, instanceData] : data.mInstancesMap)
	{
		if (!instanceData.getBlas()->isReady() && std::find(toBuild.begin(), toBuild.end(), instanceData.getBlas()) == toBuild.end())
		{
			toBuild.emplace_back(instanceData.getBlas());
		}
	}

	if (toBuild.empty())
		return;

	std::vector<vk::AccelerationStructureBuildGeometryInfoKHR> buildInfos(toBuild.size());
	vk::DeviceSize maxScratch = 0;

	for (u64 i = 0; i < toBuild.size(); ++i)
	{
		auto& blas = *toBuild[i];
		const auto& blasData = blas.getBlasData();

		blas.getBuildInfo(buildInfos[i]);
		buildInfos[i].flags = flags;

		// Query both the size of the finished acceleration structure and the  amount of scratch memory
		// needed (both written to sizeInfo). The `vkgetAccelerationStructureBuildSizesKHR` function
		// computes the worst case memory requirements based on the user-reported max number of
		// primitives. Later, compaction can fix this potential inefficiency.
		vk::AccelerationStructureBuildSizesInfoKHR sizeInfo {};
		mDevice->getAccelerationStructureBuildSizesKHR(vk::AccelerationStructureBuildTypeKHR::eDevice, &buildInfos[i], &blasData.mInfos.primitiveCount, &sizeInfo);

		std::unique_ptr<DeviceBuffer> buffer = std::make_unique<DeviceBuffer>(
			mDevice,
			sizeInfo.accelerationStructureSize,
			BufferUsage::SHADER_DEVICE_ADDRESS | BufferUsage::ACCELERATION_STRUCTURE_STORAGE);

		// Create acceleration structure object. Not yet bound to memory.
		vk::AccelerationStructureCreateInfoKHR createInfo;
		createInfo.type = vk::AccelerationStructureTypeKHR::eBottomLevel;
		createInfo.size = sizeInfo.accelerationStructureSize; // Will be used to allocate memory.
		createInfo.buffer = buffer->getBuffer();

		// Actual allocation of buffer and acceleration structure. Note: This relies on createInfo.offset == 0
		// and fills in createInfo.buffer with the buffer allocated to store the BLAS. The underlying
		// vkCreateAccelerationStructureKHR call then consumes the buffer value
		vk::UniqueAccelerationStructureKHR as;
		ERM_VK_CHECK_AND_ASSIGN(as, mDevice->createAccelerationStructureKHRUnique(createInfo));
		blas.setAS(std::move(as));
		blas.setBuffer(std::move(buffer));
		buildInfos[i].dstAccelerationStructure = blas.getAS(); // Setting the where the build lands

		// Keeping info
		maxScratch = std::max(maxScratch, sizeInfo.buildScratchSize);
	}

	DeviceBuffer scratchBuffer(mDevice, maxScratch, BufferUsage::SHADER_DEVICE_ADDRESS | BufferUsage::STORAGE_BUFFER);

	vk::BufferDeviceAddressInfo bufferInfo;
	bufferInfo.buffer = scratchBuffer.getBuffer();
	VkDeviceAddress scratchAddress = mDevice->getBufferAddress(&bufferInfo);

	// Is compaction requested?
	bool doCompaction = (flags & vk::BuildAccelerationStructureFlagBitsKHR::eAllowCompaction) == vk::BuildAccelerationStructureFlagBitsKHR::eAllowCompaction;

	// Allocate a query pool for storing the needed size for every BLAS compaction.
	vk::QueryPoolCreateInfo qpci;
	qpci.queryCount = static_cast<u32>(toBuild.size());
	qpci.queryType = vk::QueryType::eAccelerationStructureCompactedSizeKHR;
	vk::UniqueQueryPool queryPool;
	ERM_VK_CHECK_AND_ASSIGN(queryPool, mDevice->createQueryPoolUnique(qpci));

	// Allocate a command pool for queue of given queue index.
	// To avoid timeout, record and submit one command buffer per AS build.
	std::vector<vk::CommandBuffer> allCmdBufs(toBuild.size());

	vk::CommandBufferAllocateInfo allocInfo = {};
	allocInfo.commandPool = mDevice.getCommandPool();
	allocInfo.level = vk::CommandBufferLevel::ePrimary;
	allocInfo.commandBufferCount = static_cast<u32>(allCmdBufs.size());

	ERM_VK_CHECK(mDevice->allocateCommandBuffers(&allocInfo, allCmdBufs.data()));

	// Building the acceleration structures
	for (u64 i = 0; i < toBuild.size(); i++)
	{
		auto& blas = *toBuild[i];
		const auto& data = blas.getBlasData();
		vk::CommandBuffer& cmdBuf = allCmdBufs[i];

		vk::CommandBufferBeginInfo beginInfo = {};
		beginInfo.flags = {};
		beginInfo.pInheritanceInfo = nullptr;

		ERM_VK_CHECK(cmdBuf.begin(beginInfo));

		// All build are using the same scratch buffer
		buildInfos[i].scratchData.deviceAddress = scratchAddress;

		// Convert user vector of offsets to vector of pointer-to-offset (required by vk).
		// Recall that this defines which (sub)section of the vertex/index arrays
		// will be built into the BLAS.
		const std::vector<const vk::AccelerationStructureBuildRangeInfoKHR*> pBuildOffset {&data.mInfos};

		// Building the AS
		cmdBuf.buildAccelerationStructuresKHR(1, &buildInfos[i], pBuildOffset.data());

		// Since the scratch buffer is reused across builds, we need a barrier to ensure one build
		// is finished before starting the next one
		vk::MemoryBarrier barrier;
		barrier.srcAccessMask = vk::AccessFlagBits::eAccelerationStructureWriteKHR;
		barrier.dstAccessMask = vk::AccessFlagBits::eAccelerationStructureReadKHR;
		cmdBuf.pipelineBarrier(
			vk::PipelineStageFlagBits::eAccelerationStructureBuildKHR,
			vk::PipelineStageFlagBits::eAccelerationStructureBuildKHR,
			{},
			1,
			&barrier,
			0,
			nullptr,
			0,
			nullptr);

		// Write compacted size to query number i.
		if (doCompaction)
		{
			cmdBuf.writeAccelerationStructuresPropertiesKHR(
				1,
				&blas.getAS(),
				vk::QueryType::eAccelerationStructureCompactedSizeKHR,
				queryPool.get(),
				static_cast<u32>(i));
		}

		ERM_VK_CHECK(cmdBuf.end());
	}
	vk::SubmitInfo sInfo;
	sInfo.commandBufferCount = static_cast<u32>(allCmdBufs.size());
	sInfo.pCommandBuffers = allCmdBufs.data();

	ERM_VK_CHECK(mDevice.getGraphicsQueue().submit(1, &sInfo, {}));
	ERM_VK_CHECK(mDevice.getGraphicsQueue().waitIdle());

	allCmdBufs.clear();

	// Compacting all BLAS
	if (doCompaction)
	{
		// get the size result back
		std::vector<vk::DeviceSize> compactSizes(toBuild.size());
		ERM_VK_CHECK(mDevice->getQueryPoolResults(
			queryPool.get(),
			0,
			static_cast<u32>(compactSizes.size()),
			compactSizes.size() * sizeof(vk::DeviceSize),
			compactSizes.data(),
			sizeof(vk::DeviceSize),
			vk::QueryResultFlagBits::eWait));

		std::vector<vk::UniqueAccelerationStructureKHR> toReplace(toBuild.size());

		vk::CommandBuffer cmdBuf = VkUtils::beginSingleTimeCommands(mDevice);

		// Compacting
		for (u64 i = 0; i < toBuild.size(); ++i)
		{
			auto& blas = *toBuild[i];
			std::unique_ptr<DeviceBuffer> buffer = std::make_unique<DeviceBuffer>(
				mDevice,
				compactSizes[i],
				BufferUsage::SHADER_DEVICE_ADDRESS | BufferUsage::ACCELERATION_STRUCTURE_STORAGE);

			// Creating a compact version of the AS
			vk::AccelerationStructureCreateInfoKHR asCreateInfo;
			asCreateInfo.size = compactSizes[i];
			asCreateInfo.type = vk::AccelerationStructureTypeKHR::eBottomLevel;
			asCreateInfo.buffer = buffer->getBuffer();
			
			vk::UniqueAccelerationStructureKHR as;
			ERM_VK_CHECK_AND_ASSIGN(as, mDevice->createAccelerationStructureKHRUnique(asCreateInfo));

			// Copy the original BLAS to a compact version
			vk::CopyAccelerationStructureInfoKHR copyInfo;
			copyInfo.src = blas.getAS();
			copyInfo.dst = as.get();
			copyInfo.mode = vk::CopyAccelerationStructureModeKHR::eCompact;

			cmdBuf.copyAccelerationStructureKHR(copyInfo);

			toReplace[i] = std::move(as);
			blas.setBuffer(std::move(buffer));
		}

		VkUtils::endSingleTimeCommands(mDevice, cmdBuf);

		for (u64 i = 0; i < toBuild.size(); ++i)
		{
			auto& blas = *toBuild[i];
			blas.setAS(std::move(toReplace[i]));
		}
	}
}

void RTRenderingResources::updateTopLevelAS(RTRenderData& data, vk::BuildAccelerationStructureFlagsKHR flags)
{
	ERM_PROFILE_FUNCTION();

	const u64 targetGeometries = data.mInstancesMap.size();
	std::vector<vk::AccelerationStructureInstanceKHR> geometryInstances;
	geometryInstances.reserve(targetGeometries);

	for (const auto& [id, instData] : data.mInstancesMap)
	{
		auto& blas = *instData.getBlas();

		auto& instance = geometryInstances.emplace_back();

		vk::AccelerationStructureDeviceAddressInfoKHR addressInfo;
		addressInfo.accelerationStructure = blas.getAS();
		vk::DeviceAddress blasAddress = mDevice->getAccelerationStructureAddressKHR(&addressInfo);

		// The matrices for the instance transforms are row-major, instead of
		// column-major in the rest of the application
		mat4 transp = glm::transpose(instData.getTransform());

		// The gInst.transform value only contains 12 values, corresponding to a 4x3
		// matrix, hence saving the last row that is anyway always (0,0,0,1). Since
		// the matrix is row-major, we simply copy the first 12 values of the
		// original 4x4 matrix
		memcpy(&instance.transform, &transp, sizeof(instance.transform));
		instance.mask = 0xFF;
		instance.instanceShaderBindingTableRecordOffset = 0;
		instance.instanceCustomIndex = id;
		instance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
		instance.accelerationStructureReference = blasAddress;
	}

	// Create a buffer holding the actual instance data (matrices++) for use by the AS builder
	vk::DeviceSize instanceDescsSizeInBytes = targetGeometries * sizeof(vk::AccelerationStructureInstanceKHR);

	// Allocate the instance buffer and copy its contents from host to device memory
	DeviceBuffer instancesBuffer(mDevice, instanceDescsSizeInBytes, BufferUsage::SHADER_DEVICE_ADDRESS | BufferUsage::ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY);

	vk::CommandBuffer cmd = VkUtils::beginSingleTimeCommands(mDevice);
	instancesBuffer.update(cmd, geometryInstances.data());

	vk::BufferDeviceAddressInfo bufferInfo;
	bufferInfo.buffer = instancesBuffer.getBuffer();
	vk::DeviceAddress instanceAddress = mDevice->getBufferAddress(bufferInfo);

	// Make sure the copy of the instance buffer are copied before triggering the
	// acceleration structure build
	vk::MemoryBarrier barrier;
	barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
	barrier.dstAccessMask = vk::AccessFlagBits::eAccelerationStructureWriteKHR;

	cmd.pipelineBarrier(
		vk::PipelineStageFlagBits::eTransfer,
		vk::PipelineStageFlagBits::eAccelerationStructureBuildKHR,
		{},
		1,
		&barrier,
		0,
		nullptr,
		0,
		nullptr);

	// Create VkAccelerationStructureGeometryInstancesDataKHR
	// This wraps a device pointer to the above uploaded instances.
	vk::AccelerationStructureGeometryInstancesDataKHR instancesVk;
	instancesVk.arrayOfPointers = VK_FALSE;
	instancesVk.data.deviceAddress = instanceAddress;

	// Put the above into a VkAccelerationStructureGeometryKHR. We need to put the
	// instances struct in a union and label it as instance data.
	vk::AccelerationStructureGeometryKHR topASGeometry;
	topASGeometry.geometryType = vk::GeometryTypeKHR::eInstances;
	topASGeometry.geometry.instances = instancesVk;

	const bool shouldUpdate = mTopLevelAS.isReady();

	// Find sizes
	vk::AccelerationStructureBuildGeometryInfoKHR buildInfo;
	buildInfo.flags = flags;
	buildInfo.geometryCount = 1;
	buildInfo.pGeometries = &topASGeometry;
	buildInfo.mode = shouldUpdate
		? vk::BuildAccelerationStructureModeKHR::eUpdate
		: vk::BuildAccelerationStructureModeKHR::eBuild;
	buildInfo.type = vk::AccelerationStructureTypeKHR::eTopLevel;
	buildInfo.srcAccelerationStructure = nullptr;

	u32 count = static_cast<u32>(targetGeometries);
	vk::AccelerationStructureBuildSizesInfoKHR sizeInfo;
	mDevice->getAccelerationStructureBuildSizesKHR(vk::AccelerationStructureBuildTypeKHR::eDevice, &buildInfo, &count, &sizeInfo);

	// Create TLAS
	if (!shouldUpdate)
	{
		std::unique_ptr<DeviceBuffer> buffer = std::make_unique<DeviceBuffer>(
			mDevice,
			sizeInfo.accelerationStructureSize,
			BufferUsage::SHADER_DEVICE_ADDRESS | BufferUsage::ACCELERATION_STRUCTURE_STORAGE);

		vk::AccelerationStructureCreateInfoKHR createInfo;
		createInfo.type = vk::AccelerationStructureTypeKHR::eTopLevel;
		createInfo.size = sizeInfo.accelerationStructureSize;
		createInfo.buffer = buffer->getBuffer();
		
		vk::UniqueAccelerationStructureKHR as;
		ERM_VK_CHECK_AND_ASSIGN(as, mDevice->createAccelerationStructureKHRUnique(createInfo));

		mTopLevelAS.setAS(std::move(as));
		mTopLevelAS.setBuffer(std::move(buffer));
	}

	// Allocate the scratch memory
	DeviceBuffer scratchBuffer(mDevice, sizeInfo.buildScratchSize, BufferUsage::STORAGE_BUFFER | BufferUsage::ACCELERATION_STRUCTURE_STORAGE | BufferUsage::SHADER_DEVICE_ADDRESS);

	bufferInfo.buffer = scratchBuffer.getBuffer();
	vk::DeviceAddress scratchAddress = mDevice->getBufferAddress(bufferInfo);

	// Update build information
	buildInfo.srcAccelerationStructure = shouldUpdate ? mTopLevelAS.getAS() : nullptr;
	buildInfo.dstAccelerationStructure = mTopLevelAS.getAS();
	buildInfo.scratchData.deviceAddress = scratchAddress;

	// Build Offsets info: n instances
	vk::AccelerationStructureBuildRangeInfoKHR buildOffsetInfo {static_cast<u32>(targetGeometries), 0, 0, 0};
	const vk::AccelerationStructureBuildRangeInfoKHR* pBuildOffsetInfo = &buildOffsetInfo;

	// Build the TLAS
	cmd.buildAccelerationStructuresKHR(1, &buildInfo, &pBuildOffsetInfo);

	VkUtils::endSingleTimeCommands(mDevice, cmd);
}

void RTRenderingResources::reload()
{
	cleanup();
	createDescriptorPool();
}

void RTRenderingResources::cleanup()
{
	mPipelineResources.reset();
	mDescriptorPool.reset();
	mTopLevelAS.reset();
}

void RTRenderingResources::createDescriptorPool()
{
	auto& frameBuffers = mRenderer.getTargetFrameBuffers(FrameBufferType::FRAME_1);

	std::array<vk::DescriptorPoolSize, 4> poolSizes {};
	poolSizes[0].type = vk::DescriptorType::eUniformBuffer;
	poolSizes[0].descriptorCount = static_cast<u32>(frameBuffers.size() * 1000);
	poolSizes[1].type = vk::DescriptorType::eCombinedImageSampler;
	poolSizes[1].descriptorCount = static_cast<u32>(frameBuffers.size() * 100);
	poolSizes[2].type = vk::DescriptorType::eStorageImage;
	poolSizes[2].descriptorCount = static_cast<u32>(frameBuffers.size() * 100);
	poolSizes[3].type = vk::DescriptorType::eAccelerationStructureKHR;
	poolSizes[3].descriptorCount = static_cast<u32>(frameBuffers.size() * 10);

	vk::DescriptorPoolCreateInfo poolInfo {};
	poolInfo.poolSizeCount = static_cast<u32>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<u32>(frameBuffers.size() * 100);
	poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
	
	ERM_VK_CHECK_AND_ASSIGN(mDescriptorPool, mDevice->createDescriptorPoolUnique(poolInfo));
}

} // namespace erm
