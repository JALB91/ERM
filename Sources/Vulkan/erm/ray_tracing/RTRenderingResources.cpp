#include "erm/ray_tracing/RTRenderingResources.h"

#include "erm/math/math.h"

#include "erm/ray_tracing/RTBlas.h"
#include "erm/ray_tracing/RTPipelineResources.h"
#include "erm/ray_tracing/RTRenderData.h"
#include "erm/ray_tracing/RTShaderProgram.h"

#include "erm/rendering/Device.h"
#include "erm/rendering/buffers/DeviceBuffer.h"
#include "erm/rendering/renderer/IRenderer.h"

#include "erm/utils/VkUtils.h"

#include <algorithm>

namespace erm {

	RTRenderingResources::RTRenderingResources(
		Device& device,
		IRenderer& renderer)
		: mDevice(device)
		, mRenderer(renderer)
	{
		CreateDescriptorPool();
		CreateCommandBuffers();
	}

	RTRenderingResources::~RTRenderingResources()
	{
		Cleanup();
	}

	void RTRenderingResources::Update(std::vector<RTRenderData*>& renderData, uint32_t imageIndex)
	{
		if (renderData.empty())
			return;

		std::vector<RTBlas*> toBuild;

		for (const auto& data : renderData)
			if (!data->mBlas->GetBuffer())
				toBuild.emplace_back(data->mBlas);

		if (!toBuild.empty())
		{
			BuildBlas(toBuild, vk::BuildAccelerationStructureFlagBitsKHR::ePreferFastTrace);
		}

		UpdateTopLevelAS(
			renderData,
			vk::BuildAccelerationStructureFlagBitsKHR::ePreferFastTrace | vk::BuildAccelerationStructureFlagBitsKHR::eAllowUpdate);
	}

	vk::CommandBuffer RTRenderingResources::UpdateCommandBuffer(std::vector<RTRenderData*>& renderData, uint32_t imageIndex)
	{
		ASSERT(!renderData.empty());

		const vk::Extent2D& extent = mRenderer.GetSwapChainExtent();

		vk::CommandBuffer& cmd = mCommandBuffers[imageIndex];
		cmd.reset({});

		vk::CommandBufferBeginInfo beginInfo = {};
		beginInfo.flags = {};
		beginInfo.pInheritanceInfo = nullptr;

		cmd.begin(beginInfo);

		RTPipelineResources& resources = GetOrCreatePipelineResources(*renderData[0]);
		resources.UpdateResources(cmd, *renderData[0], imageIndex);
		resources.UpdateCommandBuffer(cmd, *renderData[0], imageIndex);

		// Size of a program identifier
		uint32_t groupSize = math::align_up(mDevice.GetRayTracingProperties().shaderGroupHandleSize, mDevice.GetRayTracingProperties().shaderGroupBaseAlignment);
		uint32_t groupStride = groupSize;
		vk::DeviceAddress sbtAddress = mDevice->getBufferAddress({resources.GetSBTBuffer()});

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

	void RTRenderingResources::BuildBlas(std::vector<RTBlas*> toBuild, vk::BuildAccelerationStructureFlagsKHR flags)
	{
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
			for (auto tt = 0; tt < blasData.mInfos.size(); tt++)
				maxPrimCount[tt] = blasData.mInfos[tt].primitiveCount; // Number of primitives/triangles

			vk::AccelerationStructureBuildSizesInfoKHR sizeInfo;
			mDevice->getAccelerationStructureBuildSizesKHR(vk::AccelerationStructureBuildTypeKHR::eDevice, buildInfos.data(), maxPrimCount.data(), &sizeInfo);

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
		vk::QueryPool queryPool = mDevice->createQueryPool(qpci);

		// Allocate a command pool for queue of given queue index.
		// To avoid timeout, record and submit one command buffer per AS build.
		std::vector<vk::CommandBuffer> allCmdBufs(toBuild.size());

		vk::CommandBufferAllocateInfo allocInfo = {};
		allocInfo.commandPool = mDevice.GetCommandPool();
		allocInfo.level = vk::CommandBufferLevel::ePrimary;
		allocInfo.commandBufferCount = static_cast<uint32_t>(allCmdBufs.size());

		allCmdBufs = mDevice->allocateCommandBuffers(allocInfo);

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
			for (size_t infoIdx = 0; infoIdx < data.mInfos.size(); infoIdx++)
				pBuildOffset[infoIdx] = &data.mInfos[infoIdx];

			// Building the AS
			cmdBuf.buildAccelerationStructuresKHR(1, buildInfos.data(), pBuildOffset.data());

			// Since the scratch buffer is reused across builds, we need a barrier to ensure one build
			// is finished before starting the next one
			vk::MemoryBarrier barrier;
			barrier.srcAccessMask = vk::AccessFlagBits::eAccelerationStructureWriteKHR;
			barrier.dstAccessMask = vk::AccessFlagBits::eAccelerationStructureReadKHR;
			cmdBuf.pipelineBarrier(
				vk::PipelineStageFlagBits::eAccelerationStructureBuildKHR,
				vk::PipelineStageFlagBits::eAccelerationStructureBuildKHR,
				vk::DependencyFlagBits::eByRegion,
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
					queryPool,
					static_cast<uint32_t>(i));
			}

			cmdBuf.end();
		}
		vk::SubmitInfo sInfo;
		sInfo.commandBufferCount = static_cast<uint32_t>(allCmdBufs.size());
		sInfo.pCommandBuffers = allCmdBufs.data();

		vk::Result result = mDevice.GetGraphicsQueue().submit(1, &sInfo, {});
		ASSERT(result == vk::Result::eSuccess);
		mDevice.GetGraphicsQueue().waitIdle();

		allCmdBufs.clear();

		// Compacting all BLAS
		if (doCompaction)
		{
			vk::CommandBuffer cmdBuf = VkUtils::BeginSingleTimeCommands(mDevice.GetCommandPool(), mDevice.GetVkDevice());

			// Get the size result back
			std::vector<vk::DeviceSize> compactSizes(toBuild.size());
			vk::Result result = mDevice->getQueryPoolResults(
				queryPool,
				0,
				static_cast<uint32_t>(compactSizes.size()),
				compactSizes.size() * sizeof(vk::DeviceSize),
				compactSizes.data(),
				sizeof(vk::DeviceSize),
				vk::QueryResultFlagBits::eWait);

			// Compacting
			for (size_t i = 0; i < toBuild.size(); i++)
			{
				RTBlas& blas = *toBuild[i];
				auto& buffer = blas.GetBuffer();
				buffer = std::make_unique<DeviceBuffer>(
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
				blas.SetAS(std::move(as));
			}
			VkUtils::EndSingleTimeCommands(mDevice.GetGraphicsQueue(), mDevice.GetCommandPool(), mDevice.GetVkDevice(), cmdBuf);
		}

		mDevice->destroyQueryPool(queryPool);
	}

	void RTRenderingResources::UpdateTopLevelAS(std::vector<RTRenderData*> data, vk::BuildAccelerationStructureFlagsKHR flags)
	{
		size_t targetGeometries = data.size();
		std::vector<vk::AccelerationStructureInstanceKHR> geometryInstances(targetGeometries);
		size_t index = 0;
		for (size_t i = 0; i < data.size(); ++i)
		{
			RTBlas& blas = *data[i]->mBlas;

			vk::AccelerationStructureInstanceKHR& instance = geometryInstances[i];

			vk::AccelerationStructureDeviceAddressInfoKHR addressInfo;
			addressInfo.accelerationStructure = blas.GetAS();
			vk::DeviceAddress blasAddress = mDevice->getAccelerationStructureAddressKHR(&addressInfo);

			// The matrices for the instance transforms are row-major, instead of
			// column-major in the rest of the application
			//nvmath::mat4f transp = nvmath::transpose(instance.transform);
			// X DAM: TODO
			math::mat4 transp = glm::transpose(glm::identity<math::mat4>());

			// The gInst.transform value only contains 12 values, corresponding to a 4x3
			// matrix, hence saving the last row that is anyway always (0,0,0,1). Since
			// the matrix is row-major, we simply copy the first 12 values of the
			// original 4x4 matrix
			memcpy(&instance.transform, &transp, sizeof(instance.transform));
			instance.instanceCustomIndex = index++;
			instance.mask = 0xFF;
			instance.instanceShaderBindingTableRecordOffset = 0;
			instance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
			instance.accelerationStructureReference = blasAddress;
		}

		// Create a buffer holding the actual instance data (matrices++) for use by the AS builder
		vk::DeviceSize instanceDescsSizeInBytes = targetGeometries * sizeof(vk::AccelerationStructureInstanceKHR);

		// Allocate the instance buffer and copy its contents from host to device memory
		mInstancesBuffer = std::make_unique<DeviceBuffer>(mDevice, instanceDescsSizeInBytes, vk::BufferUsageFlagBits::eShaderDeviceAddress);

		vk::CommandBuffer cmd = VkUtils::BeginSingleTimeCommands(mDevice.GetCommandPool(), mDevice.GetVkDevice());
		mInstancesBuffer->Update(cmd, geometryInstances.data());

		vk::BufferDeviceAddressInfo bufferInfo;
		bufferInfo.buffer = mInstancesBuffer->GetBuffer();
		vk::DeviceAddress instanceAddress = mDevice->getBufferAddress(bufferInfo);

		// Make sure the copy of the instance buffer are copied before triggering the
		// acceleration structure build
		vk::MemoryBarrier barrier;
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eAccelerationStructureWriteKHR;

		cmd.pipelineBarrier(
			vk::PipelineStageFlagBits::eTransfer,
			vk::PipelineStageFlagBits::eAccelerationStructureBuildKHR,
			vk::DependencyFlagBits::eByRegion,
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

	RTPipelineResources& RTRenderingResources::GetOrCreatePipelineResources(const RTRenderData& renderData)
	{
		if (!mPipelineResources || mPipelineResources->GetRenderConfigs().mShaderProgram != renderData.mRenderConfigs.mShaderProgram)
			mPipelineResources = std::make_unique<RTPipelineResources>(
				mDevice,
				mRenderer,
				renderData.mRenderConfigs,
				mDescriptorPool.get(),
				&mTopLevelAS.GetAS());
		return *mPipelineResources;
	}

	void RTRenderingResources::Refresh()
	{
		/*if (mRenderConfigs.mShaderProgram->NeedsReload())
			Reload();

		mRenderConfigs.mShaderProgram->OnReloaded();*/
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
		mDevice->freeCommandBuffers(
			mDevice.GetCommandPool(),
			static_cast<uint32_t>(mCommandBuffers.size()),
			mCommandBuffers.data());
		mCommandBuffers.clear();
		mDescriptorPool.reset();
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

		mCommandBuffers = mDevice->allocateCommandBuffers(allocInfo);
	}

} // namespace erm
