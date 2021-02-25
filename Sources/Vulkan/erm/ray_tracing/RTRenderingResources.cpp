#include "erm/ray_tracing/RTRenderingResources.h"

#include "erm/math/math.h"

#include "erm/ray_tracing/RTBlas.h"
#include "erm/ray_tracing/RTPipelineResources.h"
#include "erm/ray_tracing/RTRenderData.h"
#include "erm/ray_tracing/RTShaderProgram.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/buffers/DeviceBuffer.h"
#include "erm/rendering/renderer/IRenderer.h"

#include "erm/utils/Profiler.h"
#include "erm/utils/VkUtils.h"

#include <algorithm>

namespace erm {

	RTRenderingResources::RTRenderingResources(
		Device& device,
		IRenderer& renderer,
		const RTRenderConfigs& renderConfigs)
		: mDevice(device)
		, mRenderer(renderer)
		, mRenderConfigs(renderConfigs)
	{
		CreateDescriptorPool();
		CreateCommandBuffers();
	}

	RTRenderingResources::~RTRenderingResources() = default;

	void RTRenderingResources::Update(std::vector<RTRenderData*>& renderData, uint32_t imageIndex)
	{
		PROFILE_FUNCTION();

		BuildBlas(renderData, vk::BuildAccelerationStructureFlagBitsKHR::ePreferFastTrace);
		UpdateTopLevelAS(
			renderData,
			vk::BuildAccelerationStructureFlagBitsKHR::ePreferFastTrace | vk::BuildAccelerationStructureFlagBitsKHR::eAllowUpdate);
	}

	vk::CommandBuffer RTRenderingResources::UpdateCommandBuffer(std::vector<RTRenderData*>& renderData, uint32_t imageIndex)
	{
		PROFILE_FUNCTION();

		ASSERT(!renderData.empty());

		const vk::Extent2D& extent = mRenderer.GetSwapChainExtent();

		vk::CommandBuffer& cmd = mCommandBuffers[imageIndex].get();
		cmd.reset({});

		vk::CommandBufferBeginInfo beginInfo = {};
		beginInfo.flags = {};
		beginInfo.pInheritanceInfo = nullptr;

		cmd.begin(beginInfo);

		RTRenderData& data = *renderData[0];
		mPipelineResources->UpdateResources(cmd, data, imageIndex);
		mPipelineResources->UpdateCommandBuffer(cmd, data, imageIndex);

		// Size of a program identifier
		uint32_t groupSize = math::align_up(mDevice.GetRayTracingProperties().shaderGroupHandleSize, mDevice.GetRayTracingProperties().shaderGroupBaseAlignment);
		uint32_t groupStride = groupSize;
		vk::DeviceAddress sbtAddress = mDevice->getBufferAddress({mPipelineResources->GetSBTBuffer()});

		using Stride = vk::StridedDeviceAddressRegionKHR;
		std::array<Stride, 4> strideAddresses {
			Stride {sbtAddress + 0u * groupSize, groupStride, groupSize * 1}, // raygen
			Stride {sbtAddress + 1u * groupSize, groupStride, groupSize * 1}, // miss
			Stride {sbtAddress + 2u * groupSize, groupStride, groupSize * 1}, // hit
			Stride {0u, 0u, 0u}}; // callable

		cmd.traceRaysKHR(
			&strideAddresses[0],
			&strideAddresses[1],
			&strideAddresses[2],
			&strideAddresses[3],
			extent.width,
			extent.height,
			1);

		cmd.end();

		return cmd;
	}

	void RTRenderingResources::BuildBlas(std::vector<RTRenderData*>& data, vk::BuildAccelerationStructureFlagsKHR flags)
	{
		PROFILE_FUNCTION();

		std::vector<RTBlas*> toBuild;
		for (RTRenderData* d : data)
			if (std::find(toBuild.begin(), toBuild.end(), d->mBlas) == toBuild.end())
				toBuild.emplace_back(d->mBlas);

		std::vector<vk::AccelerationStructureBuildGeometryInfoKHR> buildInfos(toBuild.size());
		vk::DeviceSize maxScratch = 0;

		for (size_t i = 0; i < toBuild.size(); ++i)
		{
			RTBlas& blas = *toBuild[i];
			const RTBlasData& blasData = blas.GetBlasData();

			blas.GetBuildInfo(buildInfos[i]);
			buildInfos[i].flags = flags;

			// Query both the size of the finished acceleration structure and the  amount of scratch memory
			// needed (both written to sizeInfo). The `vkGetAccelerationStructureBuildSizesKHR` function
			// computes the worst case memory requirements based on the user-reported max number of
			// primitives. Later, compaction can fix this potential inefficiency.
			std::vector<uint32_t> maxPrimCount(blasData.mInfos.size());
			for (auto tt = 0; tt < blasData.mInfos.size(); ++tt)
				maxPrimCount[tt] = blasData.mInfos[tt].primitiveCount; // Number of primitives/triangles

			vk::AccelerationStructureBuildSizesInfoKHR sizeInfo {};
			mDevice->getAccelerationStructureBuildSizesKHR(vk::AccelerationStructureBuildTypeKHR::eDevice, &buildInfos[i], maxPrimCount.data(), &sizeInfo);

			auto& buffer = blas.GetBuffer();
			buffer = std::make_unique<DeviceBuffer>(
				mDevice,
				sizeInfo.accelerationStructureSize,
				vk::BufferUsageFlagBits::eShaderDeviceAddress | vk::BufferUsageFlagBits::eAccelerationStructureStorageKHR);

			// Create acceleration structure object. Not yet bound to memory.
			vk::AccelerationStructureCreateInfoKHR createInfo;
			createInfo.type = vk::AccelerationStructureTypeKHR::eBottomLevel;
			createInfo.size = sizeInfo.accelerationStructureSize; // Will be used to allocate memory.
			createInfo.buffer = buffer->GetBuffer();

			// Actual allocation of buffer and acceleration structure. Note: This relies on createInfo.offset == 0
			// and fills in createInfo.buffer with the buffer allocated to store the BLAS. The underlying
			// vkCreateAccelerationStructureKHR call then consumes the buffer value.
			blas.SetAS(mDevice->createAccelerationStructureKHRUnique(createInfo));
			buildInfos[i].dstAccelerationStructure = blas.GetAS(); // Setting the where the build lands

			// Keeping info
			maxScratch = std::max(maxScratch, sizeInfo.buildScratchSize);
		}

		DeviceBuffer scratchBuffer(mDevice, maxScratch, vk::BufferUsageFlagBits::eShaderDeviceAddress | vk::BufferUsageFlagBits::eStorageBuffer);

		vk::BufferDeviceAddressInfo bufferInfo;
		bufferInfo.buffer = scratchBuffer.GetBuffer();
		VkDeviceAddress scratchAddress = mDevice->getBufferAddress(&bufferInfo);

		// Is compaction requested?
		bool doCompaction = (flags & vk::BuildAccelerationStructureFlagBitsKHR::eAllowCompaction) == vk::BuildAccelerationStructureFlagBitsKHR::eAllowCompaction;

		// Allocate a query pool for storing the needed size for every BLAS compaction.
		vk::QueryPoolCreateInfo qpci;
		qpci.queryCount = static_cast<uint32_t>(toBuild.size());
		qpci.queryType = vk::QueryType::eAccelerationStructureCompactedSizeKHR;
		vk::UniqueQueryPool queryPool = mDevice->createQueryPoolUnique(qpci);

		// Allocate a command pool for queue of given queue index.
		// To avoid timeout, record and submit one command buffer per AS build.
		std::vector<vk::CommandBuffer> allCmdBufs(toBuild.size());

		vk::CommandBufferAllocateInfo allocInfo = {};
		allocInfo.commandPool = mDevice.GetCommandPool();
		allocInfo.level = vk::CommandBufferLevel::ePrimary;
		allocInfo.commandBufferCount = static_cast<uint32_t>(allCmdBufs.size());

		VK_CHECK(mDevice->allocateCommandBuffers(&allocInfo, allCmdBufs.data()));

		// Building the acceleration structures
		for (size_t i = 0; i < toBuild.size(); i++)
		{
			RTBlas& blas = *toBuild[i];
			const RTBlasData& data = blas.GetBlasData();
			vk::CommandBuffer& cmdBuf = allCmdBufs[i];

			vk::CommandBufferBeginInfo beginInfo = {};
			beginInfo.flags = {};
			beginInfo.pInheritanceInfo = nullptr;

			cmdBuf.begin(beginInfo);

			// All build are using the same scratch buffer
			buildInfos[i].scratchData.deviceAddress = scratchAddress;

			// Convert user vector of offsets to vector of pointer-to-offset (required by vk).
			// Recall that this defines which (sub)section of the vertex/index arrays
			// will be built into the BLAS.
			std::vector<const vk::AccelerationStructureBuildRangeInfoKHR*> pBuildOffset(data.mInfos.size());
			for (size_t infoIdx = 0; infoIdx < data.mInfos.size(); ++infoIdx)
				pBuildOffset[infoIdx] = &data.mInfos[infoIdx];

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
					&blas.GetAS(),
					vk::QueryType::eAccelerationStructureCompactedSizeKHR,
					queryPool.get(),
					static_cast<uint32_t>(i));
			}

			cmdBuf.end();
		}
		vk::SubmitInfo sInfo;
		sInfo.commandBufferCount = static_cast<uint32_t>(allCmdBufs.size());
		sInfo.pCommandBuffers = allCmdBufs.data();

		VK_CHECK(mDevice.GetGraphicsQueue().submit(1, &sInfo, {}));
		mDevice.GetGraphicsQueue().waitIdle();

		allCmdBufs.clear();

		// Compacting all BLAS
		if (doCompaction)
		{
			// Get the size result back
			std::vector<vk::DeviceSize> compactSizes(toBuild.size());
			VK_CHECK(mDevice->getQueryPoolResults(
				queryPool.get(),
				0,
				static_cast<uint32_t>(compactSizes.size()),
				compactSizes.size() * sizeof(vk::DeviceSize),
				compactSizes.data(),
				sizeof(vk::DeviceSize),
				vk::QueryResultFlagBits::eWait));

			std::vector<vk::UniqueAccelerationStructureKHR> toDelete(toBuild.size());

			vk::CommandBuffer cmdBuf = VkUtils::BeginSingleTimeCommands(mDevice.GetCommandPool(), mDevice.GetVkDevice());

			// Compacting
			for (size_t i = 0; i < toBuild.size(); ++i)
			{
				RTBlas& blas = *toBuild[i];
				std::unique_ptr<DeviceBuffer> buffer = std::make_unique<DeviceBuffer>(
					mDevice,
					compactSizes[i],
					vk::BufferUsageFlagBits::eShaderDeviceAddress | vk::BufferUsageFlagBits::eAccelerationStructureStorageKHR);

				// Creating a compact version of the AS
				vk::AccelerationStructureCreateInfoKHR asCreateInfo;
				asCreateInfo.size = compactSizes[i];
				asCreateInfo.type = vk::AccelerationStructureTypeKHR::eBottomLevel;
				asCreateInfo.buffer = buffer->GetBuffer();
				auto as = mDevice->createAccelerationStructureKHRUnique(asCreateInfo);

				// Copy the original BLAS to a compact version
				vk::CopyAccelerationStructureInfoKHR copyInfo;
				copyInfo.src = blas.GetAS();
				copyInfo.dst = as.get();
				copyInfo.mode = vk::CopyAccelerationStructureModeKHR::eCompact;

				cmdBuf.copyAccelerationStructureKHR(copyInfo);

				toDelete[i] = std::move(blas.GetASUnique());
				blas.SetAS(std::move(as));
				blas.GetBuffer() = std::move(buffer);
			}

			VkUtils::EndSingleTimeCommands(mDevice.GetGraphicsQueue(), mDevice.GetCommandPool(), mDevice.GetVkDevice(), cmdBuf);
		}
	}

	void RTRenderingResources::UpdateTopLevelAS(std::vector<RTRenderData*>& data, vk::BuildAccelerationStructureFlagsKHR flags)
	{
		PROFILE_FUNCTION();

		const size_t targetGeometries = data.size();
		std::vector<vk::AccelerationStructureInstanceKHR> geometryInstances(targetGeometries);

		for (size_t i = 0; i < data.size(); ++i)
		{
			RTBlas& blas = *data[i]->mBlas;

			vk::AccelerationStructureInstanceKHR& instance = geometryInstances[i];

			vk::AccelerationStructureDeviceAddressInfoKHR addressInfo;
			addressInfo.accelerationStructure = blas.GetAS();
			vk::DeviceAddress blasAddress = mDevice->getAccelerationStructureAddressKHR(&addressInfo);

			// The matrices for the instance transforms are row-major, instead of
			// column-major in the rest of the application
			math::mat4 transp = glm::transpose(data[i]->mTransform);

			// The gInst.transform value only contains 12 values, corresponding to a 4x3
			// matrix, hence saving the last row that is anyway always (0,0,0,1). Since
			// the matrix is row-major, we simply copy the first 12 values of the
			// original 4x4 matrix
			memcpy(&instance.transform, &transp, sizeof(instance.transform));
			instance.mask = 0xFF;
			instance.instanceShaderBindingTableRecordOffset = 0;
			instance.instanceCustomIndex = i;
			instance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
			instance.accelerationStructureReference = blasAddress;
		}

		// Create a buffer holding the actual instance data (matrices++) for use by the AS builder
		vk::DeviceSize instanceDescsSizeInBytes = targetGeometries * sizeof(vk::AccelerationStructureInstanceKHR);

		// Allocate the instance buffer and copy its contents from host to device memory
		DeviceBuffer instancesBuffer(mDevice, instanceDescsSizeInBytes, vk::BufferUsageFlagBits::eShaderDeviceAddress);

		vk::CommandBuffer cmd = VkUtils::BeginSingleTimeCommands(mDevice.GetCommandPool(), mDevice.GetVkDevice());
		instancesBuffer.Update(cmd, geometryInstances.data());

		vk::BufferDeviceAddressInfo bufferInfo;
		bufferInfo.buffer = instancesBuffer.GetBuffer();
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

		const bool shouldUpdate = mTopLevelAS.GetBuffer() != nullptr;

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

		uint32_t count = static_cast<uint32_t>(targetGeometries);
		vk::AccelerationStructureBuildSizesInfoKHR sizeInfo;
		mDevice->getAccelerationStructureBuildSizesKHR(vk::AccelerationStructureBuildTypeKHR::eDevice, &buildInfo, &count, &sizeInfo);

		// Create TLAS
		if (!shouldUpdate)
		{
			auto& buffer = mTopLevelAS.GetBuffer();
			buffer = std::make_unique<DeviceBuffer>(
				mDevice,
				sizeInfo.accelerationStructureSize,
				vk::BufferUsageFlagBits::eShaderDeviceAddress | vk::BufferUsageFlagBits::eAccelerationStructureStorageKHR);

			vk::AccelerationStructureCreateInfoKHR createInfo;
			createInfo.type = vk::AccelerationStructureTypeKHR::eTopLevel;
			createInfo.size = sizeInfo.accelerationStructureSize;
			createInfo.buffer = buffer->GetBuffer();

			mTopLevelAS.SetAS(mDevice->createAccelerationStructureKHRUnique(createInfo));

			mPipelineResources = std::make_unique<RTPipelineResources>(
				mDevice,
				mRenderer,
				mRenderConfigs,
				mDescriptorPool.get(),
				&mTopLevelAS.GetAS());
		}

		// Allocate the scratch memory
		DeviceBuffer scratchBuffer(mDevice, sizeInfo.buildScratchSize, vk::BufferUsageFlagBits::eAccelerationStructureStorageKHR | vk::BufferUsageFlagBits::eShaderDeviceAddress);

		bufferInfo.buffer = scratchBuffer.GetBuffer();
		vk::DeviceAddress scratchAddress = mDevice->getBufferAddress(bufferInfo);

		// Update build information
		buildInfo.srcAccelerationStructure = shouldUpdate ? mTopLevelAS.GetAS() : nullptr;
		buildInfo.dstAccelerationStructure = mTopLevelAS.GetAS();
		buildInfo.scratchData.deviceAddress = scratchAddress;

		// Build Offsets info: n instances
		vk::AccelerationStructureBuildRangeInfoKHR buildOffsetInfo {static_cast<uint32_t>(targetGeometries), 0, 0, 0};
		const vk::AccelerationStructureBuildRangeInfoKHR* pBuildOffsetInfo = &buildOffsetInfo;

		// Build the TLAS
		cmd.buildAccelerationStructuresKHR(1, &buildInfo, &pBuildOffsetInfo);

		VkUtils::EndSingleTimeCommands(mDevice.GetGraphicsQueue(), mDevice.GetCommandPool(), mDevice.GetVkDevice(), cmd);
	}

	void RTRenderingResources::Refresh()
	{
		if (mRenderConfigs.mShaderProgram->NeedsReload())
		{
			Reload();
			mRenderConfigs.mShaderProgram->OnReloaded();
		}
	}

	void RTRenderingResources::Reload()
	{
		Cleanup();
		CreateDescriptorPool();
		CreateCommandBuffers();
	}

	void RTRenderingResources::Cleanup()
	{
		mPipelineResources.reset();
		mCommandBuffers.clear();
		mDescriptorPool.reset();
		mTopLevelAS.GetBuffer().reset();
		mTopLevelAS.SetAS({});
	}

	void RTRenderingResources::CreateDescriptorPool()
	{
		const std::vector<vk::ImageView>& swapChainImageViews = mRenderer.GetSwapChainImageViews();

		std::array<vk::DescriptorPoolSize, 4> poolSizes {};
		poolSizes[0].type = vk::DescriptorType::eUniformBuffer;
		poolSizes[0].descriptorCount = static_cast<uint32_t>(swapChainImageViews.size() * 1000);
		poolSizes[1].type = vk::DescriptorType::eCombinedImageSampler;
		poolSizes[1].descriptorCount = static_cast<uint32_t>(swapChainImageViews.size() * 100);
		poolSizes[2].type = vk::DescriptorType::eStorageImage;
		poolSizes[2].descriptorCount = static_cast<uint32_t>(swapChainImageViews.size() * 10);
		poolSizes[3].type = vk::DescriptorType::eAccelerationStructureKHR;
		poolSizes[3].descriptorCount = static_cast<uint32_t>(swapChainImageViews.size());

		vk::DescriptorPoolCreateInfo poolInfo {};
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(swapChainImageViews.size() * 100);
		poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;

		mDescriptorPool = mDevice->createDescriptorPoolUnique(poolInfo);
	}

	void RTRenderingResources::CreateCommandBuffers()
	{
		const std::vector<vk::ImageView>& swapChainImageViews = mRenderer.GetSwapChainImageViews();

		mCommandBuffers.resize(swapChainImageViews.size());

		vk::CommandBufferAllocateInfo allocInfo = {};
		allocInfo.commandPool = mDevice.GetCommandPool();
		allocInfo.level = vk::CommandBufferLevel::ePrimary;
		allocInfo.commandBufferCount = static_cast<uint32_t>(mCommandBuffers.size());

		mCommandBuffers = mDevice->allocateCommandBuffersUnique(allocInfo);
	}

} // namespace erm
