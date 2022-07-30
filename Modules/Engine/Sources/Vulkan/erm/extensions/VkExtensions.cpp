#include "erm/extensions/VkExtensions.h"

#include <assert.h>

/* loaders */
/* /////////////////////////////////// */
#if VK_EXT_debug_marker
static PFN_vkDebugMarkerSetObjectTagEXT pfn_vkDebugMarkerSetObjectTagEXT = 0;
static PFN_vkDebugMarkerSetObjectNameEXT pfn_vkDebugMarkerSetObjectNameEXT = 0;
static PFN_vkCmdDebugMarkerBeginEXT pfn_vkCmdDebugMarkerBeginEXT = 0;
static PFN_vkCmdDebugMarkerEndEXT pfn_vkCmdDebugMarkerEndEXT = 0;
static PFN_vkCmdDebugMarkerInsertEXT pfn_vkCmdDebugMarkerInsertEXT = 0;

#	if defined(ERM_MAC)
#		pragma clang diagnostic push
#		pragma clang diagnostic ignored "-Wall"
#		pragma clang diagnostic ignored "-pedantic-errors"
#		pragma clang diagnostic ignored "-pedantic"
#		pragma clang diagnostic ignored "-Wextra"
#	endif

VKAPI_ATTR VkResult VKAPI_CALL vkDebugMarkerSetObjectTagEXT(
	VkDevice device,
	const VkDebugMarkerObjectTagInfoEXT* pTagInfo)
{
	assert(pfn_vkDebugMarkerSetObjectTagEXT);
	return pfn_vkDebugMarkerSetObjectTagEXT(device, pTagInfo);
}
VKAPI_ATTR VkResult VKAPI_CALL vkDebugMarkerSetObjectNameEXT(
	VkDevice device,
	const VkDebugMarkerObjectNameInfoEXT* pNameInfo)
{
	assert(pfn_vkDebugMarkerSetObjectNameEXT);
	return pfn_vkDebugMarkerSetObjectNameEXT(device, pNameInfo);
}
VKAPI_ATTR void VKAPI_CALL vkCmdDebugMarkerBeginEXT(
	VkCommandBuffer commandBuffer,
	const VkDebugMarkerMarkerInfoEXT* pMarkerInfo)
{
	assert(pfn_vkCmdDebugMarkerBeginEXT);
	pfn_vkCmdDebugMarkerBeginEXT(commandBuffer, pMarkerInfo);
}
VKAPI_ATTR void VKAPI_CALL vkCmdDebugMarkerEndEXT(
	VkCommandBuffer commandBuffer)
{
	assert(pfn_vkCmdDebugMarkerEndEXT);
	pfn_vkCmdDebugMarkerEndEXT(commandBuffer);
}
VKAPI_ATTR void VKAPI_CALL vkCmdDebugMarkerInsertEXT(
	VkCommandBuffer commandBuffer,
	const VkDebugMarkerMarkerInfoEXT* pMarkerInfo)
{
	assert(pfn_vkCmdDebugMarkerInsertEXT);
	pfn_vkCmdDebugMarkerInsertEXT(commandBuffer, pMarkerInfo);
}

int has_VK_EXT_debug_marker = 0;
int load_VK_EXT_debug_marker(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr)
{
	pfn_vkDebugMarkerSetObjectTagEXT = (PFN_vkDebugMarkerSetObjectTagEXT)getDeviceProcAddr(device, "vkDebugMarkerSetObjectTagEXT");
	pfn_vkDebugMarkerSetObjectNameEXT = (PFN_vkDebugMarkerSetObjectNameEXT)getDeviceProcAddr(device, "vkDebugMarkerSetObjectNameEXT");
	pfn_vkCmdDebugMarkerBeginEXT = (PFN_vkCmdDebugMarkerBeginEXT)getDeviceProcAddr(device, "vkCmdDebugMarkerBeginEXT");
	pfn_vkCmdDebugMarkerEndEXT = (PFN_vkCmdDebugMarkerEndEXT)getDeviceProcAddr(device, "vkCmdDebugMarkerEndEXT");
	pfn_vkCmdDebugMarkerInsertEXT = (PFN_vkCmdDebugMarkerInsertEXT)getDeviceProcAddr(device, "vkCmdDebugMarkerInsertEXT");
	int success = 1;
	success = success && (pfn_vkDebugMarkerSetObjectTagEXT != 0);
	success = success && (pfn_vkDebugMarkerSetObjectNameEXT != 0);
	success = success && (pfn_vkCmdDebugMarkerBeginEXT != 0);
	success = success && (pfn_vkCmdDebugMarkerEndEXT != 0);
	success = success && (pfn_vkCmdDebugMarkerInsertEXT != 0);
	has_VK_EXT_debug_marker = success;
	return success;
}
#endif

/* /////////////////////////////////// */
#if VK_KHR_external_memory_win32 && defined(VK_USE_PLATFORM_WIN32_KHR)
static PFN_vkGetMemoryWin32HandleKHR pfn_vkGetMemoryWin32HandleKHR = 0;
static PFN_vkGetMemoryWin32HandlePropertiesKHR pfn_vkGetMemoryWin32HandlePropertiesKHR = 0;

VKAPI_ATTR VkResult VKAPI_CALL vkGetMemoryWin32HandleKHR(
	VkDevice device,
	const VkMemoryGetWin32HandleInfoKHR* pGetWin32HandleInfo,
	HANDLE* pHandle)
{
	assert(pfn_vkGetMemoryWin32HandleKHR);
	return pfn_vkGetMemoryWin32HandleKHR(device, pGetWin32HandleInfo, pHandle);
}
VKAPI_ATTR VkResult VKAPI_CALL vkGetMemoryWin32HandlePropertiesKHR(
	VkDevice device,
	VkExternalMemoryHandleTypeFlagBits handleType,
	HANDLE handle,
	VkMemoryWin32HandlePropertiesKHR* pMemoryWin32HandleProperties)
{
	assert(pfn_vkGetMemoryWin32HandlePropertiesKHR);
	return pfn_vkGetMemoryWin32HandlePropertiesKHR(device, handleType, handle, pMemoryWin32HandleProperties);
}

int has_VK_KHR_external_memory_win32 = 0;
int load_VK_KHR_external_memory_win32(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr)
{
	pfn_vkGetMemoryWin32HandleKHR = (PFN_vkGetMemoryWin32HandleKHR)getDeviceProcAddr(device, "vkGetMemoryWin32HandleKHR");
	pfn_vkGetMemoryWin32HandlePropertiesKHR = (PFN_vkGetMemoryWin32HandlePropertiesKHR)getDeviceProcAddr(device, "vkGetMemoryWin32HandlePropertiesKHR");
	int success = 1;
	success = success && (pfn_vkGetMemoryWin32HandleKHR != 0);
	success = success && (pfn_vkGetMemoryWin32HandlePropertiesKHR != 0);
	has_VK_KHR_external_memory_win32 = success;
	return success;
}
#endif

/* /////////////////////////////////// */
#if VK_KHR_external_memory_fd
static PFN_vkGetMemoryFdKHR pfn_vkGetMemoryFdKHR = 0;
static PFN_vkGetMemoryFdPropertiesKHR pfn_vkGetMemoryFdPropertiesKHR = 0;

VKAPI_ATTR VkResult VKAPI_CALL vkGetMemoryFdKHR(
	VkDevice device,
	const VkMemoryGetFdInfoKHR* pGetFdInfo,
	int* pFd)
{
	assert(pfn_vkGetMemoryFdKHR);
	return pfn_vkGetMemoryFdKHR(device, pGetFdInfo, pFd);
}
VKAPI_ATTR VkResult VKAPI_CALL vkGetMemoryFdPropertiesKHR(
	VkDevice device,
	VkExternalMemoryHandleTypeFlagBits handleType,
	int fd,
	VkMemoryFdPropertiesKHR* pMemoryFdProperties)
{
	assert(pfn_vkGetMemoryFdPropertiesKHR);
	return pfn_vkGetMemoryFdPropertiesKHR(device, handleType, fd, pMemoryFdProperties);
}

int has_VK_KHR_external_memory_fd = 0;
int load_VK_KHR_external_memory_fd(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr)
{
	pfn_vkGetMemoryFdKHR = (PFN_vkGetMemoryFdKHR)getDeviceProcAddr(device, "vkGetMemoryFdKHR");
	pfn_vkGetMemoryFdPropertiesKHR = (PFN_vkGetMemoryFdPropertiesKHR)getDeviceProcAddr(device, "vkGetMemoryFdPropertiesKHR");
	int success = 1;
	success = success && (pfn_vkGetMemoryFdKHR != 0);
	success = success && (pfn_vkGetMemoryFdPropertiesKHR != 0);
	has_VK_KHR_external_memory_fd = success;
	return success;
}
#endif

/* /////////////////////////////////// */
#if VK_KHR_external_semaphore_win32 && defined(VK_USE_PLATFORM_WIN32_KHR)
static PFN_vkImportSemaphoreWin32HandleKHR pfn_vkImportSemaphoreWin32HandleKHR = 0;
static PFN_vkGetSemaphoreWin32HandleKHR pfn_vkGetSemaphoreWin32HandleKHR = 0;

VKAPI_ATTR VkResult VKAPI_CALL vkImportSemaphoreWin32HandleKHR(
	VkDevice device,
	const VkImportSemaphoreWin32HandleInfoKHR* pImportSemaphoreWin32HandleInfo)
{
	assert(pfn_vkImportSemaphoreWin32HandleKHR);
	return pfn_vkImportSemaphoreWin32HandleKHR(device, pImportSemaphoreWin32HandleInfo);
}
VKAPI_ATTR VkResult VKAPI_CALL vkGetSemaphoreWin32HandleKHR(
	VkDevice device,
	const VkSemaphoreGetWin32HandleInfoKHR* pGetWin32HandleInfo,
	HANDLE* pHandle)
{
	assert(pfn_vkGetSemaphoreWin32HandleKHR);
	return pfn_vkGetSemaphoreWin32HandleKHR(device, pGetWin32HandleInfo, pHandle);
}

int has_VK_KHR_external_semaphore_win32 = 0;
int load_VK_KHR_external_semaphore_win32(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr)
{
	pfn_vkImportSemaphoreWin32HandleKHR = (PFN_vkImportSemaphoreWin32HandleKHR)getDeviceProcAddr(device, "vkImportSemaphoreWin32HandleKHR");
	pfn_vkGetSemaphoreWin32HandleKHR = (PFN_vkGetSemaphoreWin32HandleKHR)getDeviceProcAddr(device, "vkGetSemaphoreWin32HandleKHR");
	int success = 1;
	success = success && (pfn_vkImportSemaphoreWin32HandleKHR != 0);
	success = success && (pfn_vkGetSemaphoreWin32HandleKHR != 0);
	has_VK_KHR_external_semaphore_win32 = success;
	return success;
}
#endif

/* /////////////////////////////////// */
#if VK_KHR_external_semaphore_fd
static PFN_vkImportSemaphoreFdKHR pfn_vkImportSemaphoreFdKHR = 0;
static PFN_vkGetSemaphoreFdKHR pfn_vkGetSemaphoreFdKHR = 0;

VKAPI_ATTR VkResult VKAPI_CALL vkImportSemaphoreFdKHR(
	VkDevice device,
	const VkImportSemaphoreFdInfoKHR* pImportSemaphoreFdInfo)
{
	assert(pfn_vkImportSemaphoreFdKHR);
	return pfn_vkImportSemaphoreFdKHR(device, pImportSemaphoreFdInfo);
}
VKAPI_ATTR VkResult VKAPI_CALL vkGetSemaphoreFdKHR(
	VkDevice device,
	const VkSemaphoreGetFdInfoKHR* pGetFdInfo,
	int* pFd)
{
	assert(pfn_vkGetSemaphoreFdKHR);
	return pfn_vkGetSemaphoreFdKHR(device, pGetFdInfo, pFd);
}

int has_VK_KHR_external_semaphore_fd = 0;
int load_VK_KHR_external_semaphore_fd(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr)
{
	pfn_vkImportSemaphoreFdKHR = (PFN_vkImportSemaphoreFdKHR)getDeviceProcAddr(device, "vkImportSemaphoreFdKHR");
	pfn_vkGetSemaphoreFdKHR = (PFN_vkGetSemaphoreFdKHR)getDeviceProcAddr(device, "vkGetSemaphoreFdKHR");
	int success = 1;
	success = success && (pfn_vkImportSemaphoreFdKHR != 0);
	success = success && (pfn_vkGetSemaphoreFdKHR != 0);
	has_VK_KHR_external_semaphore_fd = success;
	return success;
}
#endif

/* /////////////////////////////////// */
#if VK_KHR_push_descriptor
static PFN_vkCmdPushDescriptorSetKHR pfn_vkCmdPushDescriptorSetKHR = 0;
static PFN_vkCmdPushDescriptorSetWithTemplateKHR pfn_vkCmdPushDescriptorSetWithTemplateKHR = 0;

VKAPI_ATTR void VKAPI_CALL vkCmdPushDescriptorSetKHR(
	VkCommandBuffer commandBuffer,
	VkPipelineBindPoint pipelineBindPoint,
	VkPipelineLayout layout,
	uint32_t set,
	uint32_t descriptorWriteCount,
	const VkWriteDescriptorSet* pDescriptorWrites)
{
	assert(pfn_vkCmdPushDescriptorSetKHR);
	pfn_vkCmdPushDescriptorSetKHR(commandBuffer, pipelineBindPoint, layout, set, descriptorWriteCount, pDescriptorWrites);
}
VKAPI_ATTR void VKAPI_CALL vkCmdPushDescriptorSetWithTemplateKHR(
	VkCommandBuffer commandBuffer,
	VkDescriptorUpdateTemplate descriptorUpdateTemplate,
	VkPipelineLayout layout,
	uint32_t set,
	const void* pData)
{
	assert(pfn_vkCmdPushDescriptorSetWithTemplateKHR);
	pfn_vkCmdPushDescriptorSetWithTemplateKHR(commandBuffer, descriptorUpdateTemplate, layout, set, pData);
}

int has_VK_KHR_push_descriptor = 0;
int load_VK_KHR_push_descriptor(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr)
{
	pfn_vkCmdPushDescriptorSetKHR = (PFN_vkCmdPushDescriptorSetKHR)getDeviceProcAddr(device, "vkCmdPushDescriptorSetKHR");
	pfn_vkCmdPushDescriptorSetWithTemplateKHR = (PFN_vkCmdPushDescriptorSetWithTemplateKHR)getDeviceProcAddr(device, "vkCmdPushDescriptorSetWithTemplateKHR");
	int success = 1;
	success = success && (pfn_vkCmdPushDescriptorSetKHR != 0);
	success = success && (pfn_vkCmdPushDescriptorSetWithTemplateKHR != 0);
	has_VK_KHR_push_descriptor = success;
	return success;
}
#endif

/* /////////////////////////////////// */
#if VK_KHR_create_renderpass2
static PFN_vkCreateRenderPass2KHR pfn_vkCreateRenderPass2KHR = 0;
static PFN_vkCmdBeginRenderPass2KHR pfn_vkCmdBeginRenderPass2KHR = 0;
static PFN_vkCmdNextSubpass2KHR pfn_vkCmdNextSubpass2KHR = 0;
static PFN_vkCmdEndRenderPass2KHR pfn_vkCmdEndRenderPass2KHR = 0;

VKAPI_ATTR VkResult VKAPI_CALL vkCreateRenderPass2KHR(
	VkDevice device,
	const VkRenderPassCreateInfo2KHR* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkRenderPass* pRenderPass)
{
	assert(pfn_vkCreateRenderPass2KHR);
	return pfn_vkCreateRenderPass2KHR(device, pCreateInfo, pAllocator, pRenderPass);
}
VKAPI_ATTR void VKAPI_CALL vkCmdBeginRenderPass2KHR(
	VkCommandBuffer commandBuffer,
	const VkRenderPassBeginInfo* pRenderPassBegin,
	const VkSubpassBeginInfoKHR* pSubpassBeginInfo)
{
	assert(pfn_vkCmdBeginRenderPass2KHR);
	pfn_vkCmdBeginRenderPass2KHR(commandBuffer, pRenderPassBegin, pSubpassBeginInfo);
}
VKAPI_ATTR void VKAPI_CALL vkCmdNextSubpass2KHR(
	VkCommandBuffer commandBuffer,
	const VkSubpassBeginInfoKHR* pSubpassBeginInfo,
	const VkSubpassEndInfoKHR* pSubpassEndInfo)
{
	assert(pfn_vkCmdNextSubpass2KHR);
	pfn_vkCmdNextSubpass2KHR(commandBuffer, pSubpassBeginInfo, pSubpassEndInfo);
}
VKAPI_ATTR void VKAPI_CALL vkCmdEndRenderPass2KHR(
	VkCommandBuffer commandBuffer,
	const VkSubpassEndInfoKHR* pSubpassEndInfo)
{
	assert(pfn_vkCmdEndRenderPass2KHR);
	pfn_vkCmdEndRenderPass2KHR(commandBuffer, pSubpassEndInfo);
}

int has_VK_KHR_create_renderpass2 = 0;
int load_VK_KHR_create_renderpass2(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr)
{
	pfn_vkCreateRenderPass2KHR = (PFN_vkCreateRenderPass2KHR)getDeviceProcAddr(device, "vkCreateRenderPass2KHR");
	pfn_vkCmdBeginRenderPass2KHR = (PFN_vkCmdBeginRenderPass2KHR)getDeviceProcAddr(device, "vkCmdBeginRenderPass2KHR");
	pfn_vkCmdNextSubpass2KHR = (PFN_vkCmdNextSubpass2KHR)getDeviceProcAddr(device, "vkCmdNextSubpass2KHR");
	pfn_vkCmdEndRenderPass2KHR = (PFN_vkCmdEndRenderPass2KHR)getDeviceProcAddr(device, "vkCmdEndRenderPass2KHR");
	int success = 1;
	success = success && (pfn_vkCreateRenderPass2KHR != 0);
	success = success && (pfn_vkCmdBeginRenderPass2KHR != 0);
	success = success && (pfn_vkCmdNextSubpass2KHR != 0);
	success = success && (pfn_vkCmdEndRenderPass2KHR != 0);
	has_VK_KHR_create_renderpass2 = success;
	return success;
}
#endif

/* /////////////////////////////////// */
#if VK_KHR_external_fence_win32 && defined(VK_USE_PLATFORM_WIN32_KHR)
static PFN_vkImportFenceWin32HandleKHR pfn_vkImportFenceWin32HandleKHR = 0;
static PFN_vkGetFenceWin32HandleKHR pfn_vkGetFenceWin32HandleKHR = 0;

VKAPI_ATTR VkResult VKAPI_CALL vkImportFenceWin32HandleKHR(
	VkDevice device,
	const VkImportFenceWin32HandleInfoKHR* pImportFenceWin32HandleInfo)
{
	assert(pfn_vkImportFenceWin32HandleKHR);
	return pfn_vkImportFenceWin32HandleKHR(device, pImportFenceWin32HandleInfo);
}
VKAPI_ATTR VkResult VKAPI_CALL vkGetFenceWin32HandleKHR(
	VkDevice device,
	const VkFenceGetWin32HandleInfoKHR* pGetWin32HandleInfo,
	HANDLE* pHandle)
{
	assert(pfn_vkGetFenceWin32HandleKHR);
	return pfn_vkGetFenceWin32HandleKHR(device, pGetWin32HandleInfo, pHandle);
}

int has_VK_KHR_external_fence_win32 = 0;
int load_VK_KHR_external_fence_win32(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr)
{
	pfn_vkImportFenceWin32HandleKHR = (PFN_vkImportFenceWin32HandleKHR)getDeviceProcAddr(device, "vkImportFenceWin32HandleKHR");
	pfn_vkGetFenceWin32HandleKHR = (PFN_vkGetFenceWin32HandleKHR)getDeviceProcAddr(device, "vkGetFenceWin32HandleKHR");
	int success = 1;
	success = success && (pfn_vkImportFenceWin32HandleKHR != 0);
	success = success && (pfn_vkGetFenceWin32HandleKHR != 0);
	has_VK_KHR_external_fence_win32 = success;
	return success;
}
#endif

/* /////////////////////////////////// */
#if VK_EXT_debug_utils
static PFN_vkSetDebugUtilsObjectNameEXT pfn_vkSetDebugUtilsObjectNameEXT = 0;
static PFN_vkSetDebugUtilsObjectTagEXT pfn_vkSetDebugUtilsObjectTagEXT = 0;
static PFN_vkQueueBeginDebugUtilsLabelEXT pfn_vkQueueBeginDebugUtilsLabelEXT = 0;
static PFN_vkQueueEndDebugUtilsLabelEXT pfn_vkQueueEndDebugUtilsLabelEXT = 0;
static PFN_vkQueueInsertDebugUtilsLabelEXT pfn_vkQueueInsertDebugUtilsLabelEXT = 0;
static PFN_vkCmdBeginDebugUtilsLabelEXT pfn_vkCmdBeginDebugUtilsLabelEXT = 0;
static PFN_vkCmdEndDebugUtilsLabelEXT pfn_vkCmdEndDebugUtilsLabelEXT = 0;
static PFN_vkCmdInsertDebugUtilsLabelEXT pfn_vkCmdInsertDebugUtilsLabelEXT = 0;
static PFN_vkCreateDebugUtilsMessengerEXT pfn_vkCreateDebugUtilsMessengerEXT = 0;
static PFN_vkDestroyDebugUtilsMessengerEXT pfn_vkDestroyDebugUtilsMessengerEXT = 0;
static PFN_vkSubmitDebugUtilsMessageEXT pfn_vkSubmitDebugUtilsMessageEXT = 0;

VKAPI_ATTR VkResult VKAPI_CALL vkSetDebugUtilsObjectNameEXT(
	VkDevice device,
	const VkDebugUtilsObjectNameInfoEXT* pNameInfo)
{
	assert(pfn_vkSetDebugUtilsObjectNameEXT);
	return pfn_vkSetDebugUtilsObjectNameEXT(device, pNameInfo);
}
VKAPI_ATTR VkResult VKAPI_CALL vkSetDebugUtilsObjectTagEXT(
	VkDevice device,
	const VkDebugUtilsObjectTagInfoEXT* pTagInfo)
{
	assert(pfn_vkSetDebugUtilsObjectTagEXT);
	return pfn_vkSetDebugUtilsObjectTagEXT(device, pTagInfo);
}
VKAPI_ATTR void VKAPI_CALL vkQueueBeginDebugUtilsLabelEXT(
	VkQueue queue,
	const VkDebugUtilsLabelEXT* pLabelInfo)
{
	assert(pfn_vkQueueBeginDebugUtilsLabelEXT);
	pfn_vkQueueBeginDebugUtilsLabelEXT(queue, pLabelInfo);
}
VKAPI_ATTR void VKAPI_CALL vkQueueEndDebugUtilsLabelEXT(
	VkQueue queue)
{
	assert(pfn_vkQueueEndDebugUtilsLabelEXT);
	pfn_vkQueueEndDebugUtilsLabelEXT(queue);
}
VKAPI_ATTR void VKAPI_CALL vkQueueInsertDebugUtilsLabelEXT(
	VkQueue queue,
	const VkDebugUtilsLabelEXT* pLabelInfo)
{
	assert(pfn_vkQueueInsertDebugUtilsLabelEXT);
	pfn_vkQueueInsertDebugUtilsLabelEXT(queue, pLabelInfo);
}
VKAPI_ATTR void VKAPI_CALL vkCmdBeginDebugUtilsLabelEXT(
	VkCommandBuffer commandBuffer,
	const VkDebugUtilsLabelEXT* pLabelInfo)
{
	assert(pfn_vkCmdBeginDebugUtilsLabelEXT);
	pfn_vkCmdBeginDebugUtilsLabelEXT(commandBuffer, pLabelInfo);
}
VKAPI_ATTR void VKAPI_CALL vkCmdEndDebugUtilsLabelEXT(
	VkCommandBuffer commandBuffer)
{
	assert(pfn_vkCmdEndDebugUtilsLabelEXT);
	pfn_vkCmdEndDebugUtilsLabelEXT(commandBuffer);
}
VKAPI_ATTR void VKAPI_CALL vkCmdInsertDebugUtilsLabelEXT(
	VkCommandBuffer commandBuffer,
	const VkDebugUtilsLabelEXT* pLabelInfo)
{
	assert(pfn_vkCmdInsertDebugUtilsLabelEXT);
	pfn_vkCmdInsertDebugUtilsLabelEXT(commandBuffer, pLabelInfo);
}
VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
	VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkDebugUtilsMessengerEXT* pMessenger)
{
	assert(pfn_vkCreateDebugUtilsMessengerEXT);
	return pfn_vkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pMessenger);
}
VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(
	VkInstance instance,
	VkDebugUtilsMessengerEXT messenger,
	const VkAllocationCallbacks* pAllocator)
{
	assert(pfn_vkDestroyDebugUtilsMessengerEXT);
	pfn_vkDestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
}
VKAPI_ATTR void VKAPI_CALL vkSubmitDebugUtilsMessageEXT(
	VkInstance instance,
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageTypes,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData)
{
	assert(pfn_vkSubmitDebugUtilsMessageEXT);
	pfn_vkSubmitDebugUtilsMessageEXT(instance, messageSeverity, messageTypes, pCallbackData);
}

int has_VK_EXT_debug_utils = 0;
int load_VK_EXT_debug_utils(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr)
{
	pfn_vkSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)getDeviceProcAddr(device, "vkSetDebugUtilsObjectNameEXT");
	pfn_vkSetDebugUtilsObjectTagEXT = (PFN_vkSetDebugUtilsObjectTagEXT)getDeviceProcAddr(device, "vkSetDebugUtilsObjectTagEXT");
	pfn_vkQueueBeginDebugUtilsLabelEXT = (PFN_vkQueueBeginDebugUtilsLabelEXT)getDeviceProcAddr(device, "vkQueueBeginDebugUtilsLabelEXT");
	pfn_vkQueueEndDebugUtilsLabelEXT = (PFN_vkQueueEndDebugUtilsLabelEXT)getDeviceProcAddr(device, "vkQueueEndDebugUtilsLabelEXT");
	pfn_vkQueueInsertDebugUtilsLabelEXT = (PFN_vkQueueInsertDebugUtilsLabelEXT)getDeviceProcAddr(device, "vkQueueInsertDebugUtilsLabelEXT");
	pfn_vkCmdBeginDebugUtilsLabelEXT = (PFN_vkCmdBeginDebugUtilsLabelEXT)getDeviceProcAddr(device, "vkCmdBeginDebugUtilsLabelEXT");
	pfn_vkCmdEndDebugUtilsLabelEXT = (PFN_vkCmdEndDebugUtilsLabelEXT)getDeviceProcAddr(device, "vkCmdEndDebugUtilsLabelEXT");
	pfn_vkCmdInsertDebugUtilsLabelEXT = (PFN_vkCmdInsertDebugUtilsLabelEXT)getDeviceProcAddr(device, "vkCmdInsertDebugUtilsLabelEXT");
	pfn_vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)getInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	pfn_vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)getInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	pfn_vkSubmitDebugUtilsMessageEXT = (PFN_vkSubmitDebugUtilsMessageEXT)getInstanceProcAddr(instance, "vkSubmitDebugUtilsMessageEXT");
	int success = 1;
	success = success && (pfn_vkSetDebugUtilsObjectNameEXT != 0);
	success = success && (pfn_vkSetDebugUtilsObjectTagEXT != 0);
	success = success && (pfn_vkQueueBeginDebugUtilsLabelEXT != 0);
	success = success && (pfn_vkQueueEndDebugUtilsLabelEXT != 0);
	success = success && (pfn_vkQueueInsertDebugUtilsLabelEXT != 0);
	success = success && (pfn_vkCmdBeginDebugUtilsLabelEXT != 0);
	success = success && (pfn_vkCmdEndDebugUtilsLabelEXT != 0);
	success = success && (pfn_vkCmdInsertDebugUtilsLabelEXT != 0);
	success = success && (pfn_vkCreateDebugUtilsMessengerEXT != 0);
	success = success && (pfn_vkDestroyDebugUtilsMessengerEXT != 0);
	success = success && (pfn_vkSubmitDebugUtilsMessageEXT != 0);
	has_VK_EXT_debug_utils = success;
	return success;
}
#endif

/* /////////////////////////////////// */
#if VK_EXT_sample_locations
static PFN_vkCmdSetSampleLocationsEXT pfn_vkCmdSetSampleLocationsEXT = 0;
static PFN_vkGetPhysicalDeviceMultisamplePropertiesEXT pfn_vkGetPhysicalDeviceMultisamplePropertiesEXT = 0;

VKAPI_ATTR void VKAPI_CALL vkCmdSetSampleLocationsEXT(
	VkCommandBuffer commandBuffer,
	const VkSampleLocationsInfoEXT* pSampleLocationsInfo)
{
	assert(pfn_vkCmdSetSampleLocationsEXT);
	pfn_vkCmdSetSampleLocationsEXT(commandBuffer, pSampleLocationsInfo);
}
VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceMultisamplePropertiesEXT(
	VkPhysicalDevice physicalDevice,
	VkSampleCountFlagBits samples,
	VkMultisamplePropertiesEXT* pMultisampleProperties)
{
	assert(pfn_vkGetPhysicalDeviceMultisamplePropertiesEXT);
	pfn_vkGetPhysicalDeviceMultisamplePropertiesEXT(physicalDevice, samples, pMultisampleProperties);
}

int has_VK_EXT_sample_locations = 0;
int load_VK_EXT_sample_locations(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr)
{
	pfn_vkCmdSetSampleLocationsEXT = (PFN_vkCmdSetSampleLocationsEXT)getDeviceProcAddr(device, "vkCmdSetSampleLocationsEXT");
	pfn_vkGetPhysicalDeviceMultisamplePropertiesEXT = (PFN_vkGetPhysicalDeviceMultisamplePropertiesEXT)getDeviceProcAddr(device, "vkGetPhysicalDeviceMultisamplePropertiesEXT");
	int success = 1;
	success = success && (pfn_vkCmdSetSampleLocationsEXT != 0);
	success = success && (pfn_vkGetPhysicalDeviceMultisamplePropertiesEXT != 0);
	has_VK_EXT_sample_locations = success;
	return success;
}
#endif

/* /////////////////////////////////// */
#if VK_KHR_acceleration_structure
static PFN_vkCreateAccelerationStructureKHR pfn_vkCreateAccelerationStructureKHR = 0;
static PFN_vkDestroyAccelerationStructureKHR pfn_vkDestroyAccelerationStructureKHR = 0;
static PFN_vkCmdBuildAccelerationStructuresKHR pfn_vkCmdBuildAccelerationStructuresKHR = 0;
static PFN_vkCmdBuildAccelerationStructuresIndirectKHR pfn_vkCmdBuildAccelerationStructuresIndirectKHR = 0;
static PFN_vkBuildAccelerationStructuresKHR pfn_vkBuildAccelerationStructuresKHR = 0;
static PFN_vkCopyAccelerationStructureKHR pfn_vkCopyAccelerationStructureKHR = 0;
static PFN_vkCopyAccelerationStructureToMemoryKHR pfn_vkCopyAccelerationStructureToMemoryKHR = 0;
static PFN_vkCopyMemoryToAccelerationStructureKHR pfn_vkCopyMemoryToAccelerationStructureKHR = 0;
static PFN_vkWriteAccelerationStructuresPropertiesKHR pfn_vkWriteAccelerationStructuresPropertiesKHR = 0;
static PFN_vkCmdCopyAccelerationStructureKHR pfn_vkCmdCopyAccelerationStructureKHR = 0;
static PFN_vkCmdCopyAccelerationStructureToMemoryKHR pfn_vkCmdCopyAccelerationStructureToMemoryKHR = 0;
static PFN_vkCmdCopyMemoryToAccelerationStructureKHR pfn_vkCmdCopyMemoryToAccelerationStructureKHR = 0;
static PFN_vkGetAccelerationStructureDeviceAddressKHR pfn_vkGetAccelerationStructureDeviceAddressKHR = 0;
static PFN_vkCmdWriteAccelerationStructuresPropertiesKHR pfn_vkCmdWriteAccelerationStructuresPropertiesKHR = 0;
static PFN_vkGetDeviceAccelerationStructureCompatibilityKHR pfn_vkGetDeviceAccelerationStructureCompatibilityKHR = 0;
static PFN_vkGetAccelerationStructureBuildSizesKHR pfn_vkGetAccelerationStructureBuildSizesKHR = 0;

VKAPI_ATTR VkResult VKAPI_CALL vkCreateAccelerationStructureKHR(
	VkDevice device,
	const VkAccelerationStructureCreateInfoKHR* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkAccelerationStructureKHR* pAccelerationStructure)
{
	assert(pfn_vkCreateAccelerationStructureKHR);
	return pfn_vkCreateAccelerationStructureKHR(device, pCreateInfo, pAllocator, pAccelerationStructure);
}
VKAPI_ATTR void VKAPI_CALL vkDestroyAccelerationStructureKHR(
	VkDevice device,
	VkAccelerationStructureKHR accelerationStructure,
	const VkAllocationCallbacks* pAllocator)
{
	assert(pfn_vkDestroyAccelerationStructureKHR);
	pfn_vkDestroyAccelerationStructureKHR(device, accelerationStructure, pAllocator);
}
VKAPI_ATTR void VKAPI_CALL vkCmdBuildAccelerationStructuresKHR(
	VkCommandBuffer commandBuffer,
	uint32_t infoCount,
	const VkAccelerationStructureBuildGeometryInfoKHR* pInfos,
	const VkAccelerationStructureBuildRangeInfoKHR* const* ppBuildRangeInfos)
{
	assert(pfn_vkCmdBuildAccelerationStructuresKHR);
	pfn_vkCmdBuildAccelerationStructuresKHR(commandBuffer, infoCount, pInfos, ppBuildRangeInfos);
}
VKAPI_ATTR void VKAPI_CALL vkCmdBuildAccelerationStructuresIndirectKHR(
	VkCommandBuffer commandBuffer,
	uint32_t infoCount,
	const VkAccelerationStructureBuildGeometryInfoKHR* pInfos,
	const VkDeviceAddress* pIndirectDeviceAddresses,
	const uint32_t* pIndirectStrides,
	const uint32_t* const* ppMaxPrimitiveCounts)
{
	assert(pfn_vkCmdBuildAccelerationStructuresIndirectKHR);
	pfn_vkCmdBuildAccelerationStructuresIndirectKHR(commandBuffer, infoCount, pInfos, pIndirectDeviceAddresses, pIndirectStrides, ppMaxPrimitiveCounts);
}
VKAPI_ATTR VkResult VKAPI_CALL vkBuildAccelerationStructuresKHR(
	VkDevice device,
	VkDeferredOperationKHR deferredOperation,
	uint32_t infoCount,
	const VkAccelerationStructureBuildGeometryInfoKHR* pInfos,
	const VkAccelerationStructureBuildRangeInfoKHR* const* ppBuildRangeInfos)
{
	assert(pfn_vkBuildAccelerationStructuresKHR);
	return pfn_vkBuildAccelerationStructuresKHR(device, deferredOperation, infoCount, pInfos, ppBuildRangeInfos);
}
VKAPI_ATTR VkResult VKAPI_CALL vkCopyAccelerationStructureKHR(
	VkDevice device,
	VkDeferredOperationKHR deferredOperation,
	const VkCopyAccelerationStructureInfoKHR* pInfo)
{
	assert(pfn_vkCopyAccelerationStructureKHR);
	return pfn_vkCopyAccelerationStructureKHR(device, deferredOperation, pInfo);
}
VKAPI_ATTR VkResult VKAPI_CALL vkCopyAccelerationStructureToMemoryKHR(
	VkDevice device,
	VkDeferredOperationKHR deferredOperation,
	const VkCopyAccelerationStructureToMemoryInfoKHR* pInfo)
{
	assert(pfn_vkCopyAccelerationStructureToMemoryKHR);
	return pfn_vkCopyAccelerationStructureToMemoryKHR(device, deferredOperation, pInfo);
}
VKAPI_ATTR VkResult VKAPI_CALL vkCopyMemoryToAccelerationStructureKHR(
	VkDevice device,
	VkDeferredOperationKHR deferredOperation,
	const VkCopyMemoryToAccelerationStructureInfoKHR* pInfo)
{
	assert(pfn_vkCopyMemoryToAccelerationStructureKHR);
	return pfn_vkCopyMemoryToAccelerationStructureKHR(device, deferredOperation, pInfo);
}
VKAPI_ATTR VkResult VKAPI_CALL vkWriteAccelerationStructuresPropertiesKHR(
	VkDevice device,
	uint32_t accelerationStructureCount,
	const VkAccelerationStructureKHR* pAccelerationStructures,
	VkQueryType queryType,
	size_t dataSize,
	void* pData,
	size_t stride)
{
	assert(pfn_vkWriteAccelerationStructuresPropertiesKHR);
	return pfn_vkWriteAccelerationStructuresPropertiesKHR(device, accelerationStructureCount, pAccelerationStructures, queryType, dataSize, pData, stride);
}
VKAPI_ATTR void VKAPI_CALL vkCmdCopyAccelerationStructureKHR(
	VkCommandBuffer commandBuffer,
	const VkCopyAccelerationStructureInfoKHR* pInfo)
{
	assert(pfn_vkCmdCopyAccelerationStructureKHR);
	pfn_vkCmdCopyAccelerationStructureKHR(commandBuffer, pInfo);
}
VKAPI_ATTR void VKAPI_CALL vkCmdCopyAccelerationStructureToMemoryKHR(
	VkCommandBuffer commandBuffer,
	const VkCopyAccelerationStructureToMemoryInfoKHR* pInfo)
{
	assert(pfn_vkCmdCopyAccelerationStructureToMemoryKHR);
	pfn_vkCmdCopyAccelerationStructureToMemoryKHR(commandBuffer, pInfo);
}
VKAPI_ATTR void VKAPI_CALL vkCmdCopyMemoryToAccelerationStructureKHR(
	VkCommandBuffer commandBuffer,
	const VkCopyMemoryToAccelerationStructureInfoKHR* pInfo)
{
	assert(pfn_vkCmdCopyMemoryToAccelerationStructureKHR);
	pfn_vkCmdCopyMemoryToAccelerationStructureKHR(commandBuffer, pInfo);
}
VKAPI_ATTR VkDeviceAddress VKAPI_CALL vkGetAccelerationStructureDeviceAddressKHR(
	VkDevice device,
	const VkAccelerationStructureDeviceAddressInfoKHR* pInfo)
{
	assert(pfn_vkGetAccelerationStructureDeviceAddressKHR);
	return pfn_vkGetAccelerationStructureDeviceAddressKHR(device, pInfo);
}
VKAPI_ATTR void VKAPI_CALL vkCmdWriteAccelerationStructuresPropertiesKHR(
	VkCommandBuffer commandBuffer,
	uint32_t accelerationStructureCount,
	const VkAccelerationStructureKHR* pAccelerationStructures,
	VkQueryType queryType,
	VkQueryPool queryPool,
	uint32_t firstQuery)
{
	assert(pfn_vkCmdWriteAccelerationStructuresPropertiesKHR);
	pfn_vkCmdWriteAccelerationStructuresPropertiesKHR(commandBuffer, accelerationStructureCount, pAccelerationStructures, queryType, queryPool, firstQuery);
}
VKAPI_ATTR void VKAPI_CALL vkGetDeviceAccelerationStructureCompatibilityKHR(
	VkDevice device,
	const VkAccelerationStructureVersionInfoKHR* pVersionInfo,
	VkAccelerationStructureCompatibilityKHR* pCompatibility)
{
	assert(pfn_vkGetDeviceAccelerationStructureCompatibilityKHR);
	pfn_vkGetDeviceAccelerationStructureCompatibilityKHR(device, pVersionInfo, pCompatibility);
}
VKAPI_ATTR void VKAPI_CALL vkGetAccelerationStructureBuildSizesKHR(
	VkDevice device,
	VkAccelerationStructureBuildTypeKHR buildType,
	const VkAccelerationStructureBuildGeometryInfoKHR* pBuildInfo,
	const uint32_t* pMaxPrimitiveCounts,
	VkAccelerationStructureBuildSizesInfoKHR* pSizeInfo)
{
	assert(pfn_vkGetAccelerationStructureBuildSizesKHR);
	pfn_vkGetAccelerationStructureBuildSizesKHR(device, buildType, pBuildInfo, pMaxPrimitiveCounts, pSizeInfo);
}

int has_VK_KHR_acceleration_structure = 0;
int load_VK_KHR_acceleration_structure(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr)
{
	pfn_vkCreateAccelerationStructureKHR = (PFN_vkCreateAccelerationStructureKHR)getDeviceProcAddr(device, "vkCreateAccelerationStructureKHR");
	pfn_vkDestroyAccelerationStructureKHR = (PFN_vkDestroyAccelerationStructureKHR)getDeviceProcAddr(device, "vkDestroyAccelerationStructureKHR");
	pfn_vkCmdBuildAccelerationStructuresKHR = (PFN_vkCmdBuildAccelerationStructuresKHR)getDeviceProcAddr(device, "vkCmdBuildAccelerationStructuresKHR");
	pfn_vkCmdBuildAccelerationStructuresIndirectKHR = (PFN_vkCmdBuildAccelerationStructuresIndirectKHR)getDeviceProcAddr(device, "vkCmdBuildAccelerationStructuresIndirectKHR");
	pfn_vkBuildAccelerationStructuresKHR = (PFN_vkBuildAccelerationStructuresKHR)getDeviceProcAddr(device, "vkBuildAccelerationStructuresKHR");
	pfn_vkCopyAccelerationStructureKHR = (PFN_vkCopyAccelerationStructureKHR)getDeviceProcAddr(device, "vkCopyAccelerationStructureKHR");
	pfn_vkCopyAccelerationStructureToMemoryKHR = (PFN_vkCopyAccelerationStructureToMemoryKHR)getDeviceProcAddr(device, "vkCopyAccelerationStructureToMemoryKHR");
	pfn_vkCopyMemoryToAccelerationStructureKHR = (PFN_vkCopyMemoryToAccelerationStructureKHR)getDeviceProcAddr(device, "vkCopyMemoryToAccelerationStructureKHR");
	pfn_vkWriteAccelerationStructuresPropertiesKHR = (PFN_vkWriteAccelerationStructuresPropertiesKHR)getDeviceProcAddr(device, "vkWriteAccelerationStructuresPropertiesKHR");
	pfn_vkCmdCopyAccelerationStructureKHR = (PFN_vkCmdCopyAccelerationStructureKHR)getDeviceProcAddr(device, "vkCmdCopyAccelerationStructureKHR");
	pfn_vkCmdCopyAccelerationStructureToMemoryKHR = (PFN_vkCmdCopyAccelerationStructureToMemoryKHR)getDeviceProcAddr(device, "vkCmdCopyAccelerationStructureToMemoryKHR");
	pfn_vkCmdCopyMemoryToAccelerationStructureKHR = (PFN_vkCmdCopyMemoryToAccelerationStructureKHR)getDeviceProcAddr(device, "vkCmdCopyMemoryToAccelerationStructureKHR");
	pfn_vkGetAccelerationStructureDeviceAddressKHR = (PFN_vkGetAccelerationStructureDeviceAddressKHR)getDeviceProcAddr(device, "vkGetAccelerationStructureDeviceAddressKHR");
	pfn_vkCmdWriteAccelerationStructuresPropertiesKHR = (PFN_vkCmdWriteAccelerationStructuresPropertiesKHR)getDeviceProcAddr(device, "vkCmdWriteAccelerationStructuresPropertiesKHR");
	pfn_vkGetDeviceAccelerationStructureCompatibilityKHR = (PFN_vkGetDeviceAccelerationStructureCompatibilityKHR)getDeviceProcAddr(device, "vkGetDeviceAccelerationStructureCompatibilityKHR");
	pfn_vkGetAccelerationStructureBuildSizesKHR = (PFN_vkGetAccelerationStructureBuildSizesKHR)getDeviceProcAddr(device, "vkGetAccelerationStructureBuildSizesKHR");
	int success = 1;
	success = success && (pfn_vkCreateAccelerationStructureKHR != 0);
	success = success && (pfn_vkDestroyAccelerationStructureKHR != 0);
	success = success && (pfn_vkCmdBuildAccelerationStructuresKHR != 0);
	success = success && (pfn_vkCmdBuildAccelerationStructuresIndirectKHR != 0);
	success = success && (pfn_vkBuildAccelerationStructuresKHR != 0);
	success = success && (pfn_vkCopyAccelerationStructureKHR != 0);
	success = success && (pfn_vkCopyAccelerationStructureToMemoryKHR != 0);
	success = success && (pfn_vkCopyMemoryToAccelerationStructureKHR != 0);
	success = success && (pfn_vkWriteAccelerationStructuresPropertiesKHR != 0);
	success = success && (pfn_vkCmdCopyAccelerationStructureKHR != 0);
	success = success && (pfn_vkCmdCopyAccelerationStructureToMemoryKHR != 0);
	success = success && (pfn_vkCmdCopyMemoryToAccelerationStructureKHR != 0);
	success = success && (pfn_vkGetAccelerationStructureDeviceAddressKHR != 0);
	success = success && (pfn_vkCmdWriteAccelerationStructuresPropertiesKHR != 0);
	success = success && (pfn_vkGetDeviceAccelerationStructureCompatibilityKHR != 0);
	success = success && (pfn_vkGetAccelerationStructureBuildSizesKHR != 0);
	has_VK_KHR_acceleration_structure = success;
	return success;
}
#endif

/* /////////////////////////////////// */
#if VK_KHR_ray_tracing_pipeline
static PFN_vkCmdTraceRaysKHR pfn_vkCmdTraceRaysKHR = 0;
static PFN_vkCreateRayTracingPipelinesKHR pfn_vkCreateRayTracingPipelinesKHR = 0;
static PFN_vkGetRayTracingShaderGroupHandlesKHR pfn_vkGetRayTracingShaderGroupHandlesKHR = 0;
static PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR pfn_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR = 0;
static PFN_vkCmdTraceRaysIndirectKHR pfn_vkCmdTraceRaysIndirectKHR = 0;
static PFN_vkGetRayTracingShaderGroupStackSizeKHR pfn_vkGetRayTracingShaderGroupStackSizeKHR = 0;
static PFN_vkCmdSetRayTracingPipelineStackSizeKHR pfn_vkCmdSetRayTracingPipelineStackSizeKHR = 0;

VKAPI_ATTR void VKAPI_CALL vkCmdTraceRaysKHR(
	VkCommandBuffer commandBuffer,
	const VkStridedDeviceAddressRegionKHR* pRaygenShaderBindingTable,
	const VkStridedDeviceAddressRegionKHR* pMissShaderBindingTable,
	const VkStridedDeviceAddressRegionKHR* pHitShaderBindingTable,
	const VkStridedDeviceAddressRegionKHR* pCallableShaderBindingTable,
	uint32_t width,
	uint32_t height,
	uint32_t depth)
{
	assert(pfn_vkCmdTraceRaysKHR);
	pfn_vkCmdTraceRaysKHR(commandBuffer, pRaygenShaderBindingTable, pMissShaderBindingTable, pHitShaderBindingTable, pCallableShaderBindingTable, width, height, depth);
}
VKAPI_ATTR VkResult VKAPI_CALL vkCreateRayTracingPipelinesKHR(
	VkDevice device,
	VkDeferredOperationKHR deferredOperation,
	VkPipelineCache pipelineCache,
	uint32_t createInfoCount,
	const VkRayTracingPipelineCreateInfoKHR* pCreateInfos,
	const VkAllocationCallbacks* pAllocator,
	VkPipeline* pPipelines)
{
	assert(pfn_vkCreateRayTracingPipelinesKHR);
	return pfn_vkCreateRayTracingPipelinesKHR(device, deferredOperation, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
}
VKAPI_ATTR VkResult VKAPI_CALL vkGetRayTracingShaderGroupHandlesKHR(
	VkDevice device,
	VkPipeline pipeline,
	uint32_t firstGroup,
	uint32_t groupCount,
	size_t dataSize,
	void* pData)
{
	assert(pfn_vkGetRayTracingShaderGroupHandlesKHR);
	return pfn_vkGetRayTracingShaderGroupHandlesKHR(device, pipeline, firstGroup, groupCount, dataSize, pData);
}
VKAPI_ATTR VkResult VKAPI_CALL vkGetRayTracingCaptureReplayShaderGroupHandlesKHR(
	VkDevice device,
	VkPipeline pipeline,
	uint32_t firstGroup,
	uint32_t groupCount,
	size_t dataSize,
	void* pData)
{
	assert(pfn_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR);
	return pfn_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR(device, pipeline, firstGroup, groupCount, dataSize, pData);
}
VKAPI_ATTR void VKAPI_CALL vkCmdTraceRaysIndirectKHR(
	VkCommandBuffer commandBuffer,
	const VkStridedDeviceAddressRegionKHR* pRaygenShaderBindingTable,
	const VkStridedDeviceAddressRegionKHR* pMissShaderBindingTable,
	const VkStridedDeviceAddressRegionKHR* pHitShaderBindingTable,
	const VkStridedDeviceAddressRegionKHR* pCallableShaderBindingTable,
	VkDeviceAddress indirectDeviceAddress)
{
	assert(pfn_vkCmdTraceRaysIndirectKHR);
	pfn_vkCmdTraceRaysIndirectKHR(commandBuffer, pRaygenShaderBindingTable, pMissShaderBindingTable, pHitShaderBindingTable, pCallableShaderBindingTable, indirectDeviceAddress);
}
VKAPI_ATTR VkDeviceSize VKAPI_CALL vkGetRayTracingShaderGroupStackSizeKHR(
	VkDevice device,
	VkPipeline pipeline,
	uint32_t group,
	VkShaderGroupShaderKHR groupShader)
{
	assert(pfn_vkGetRayTracingShaderGroupStackSizeKHR);
	return pfn_vkGetRayTracingShaderGroupStackSizeKHR(device, pipeline, group, groupShader);
}
VKAPI_ATTR void VKAPI_CALL vkCmdSetRayTracingPipelineStackSizeKHR(
	VkCommandBuffer commandBuffer,
	uint32_t pipelineStackSize)
{
	assert(pfn_vkCmdSetRayTracingPipelineStackSizeKHR);
	pfn_vkCmdSetRayTracingPipelineStackSizeKHR(commandBuffer, pipelineStackSize);
}

int has_VK_KHR_ray_tracing_pipeline = 0;
int load_VK_KHR_ray_tracing_pipeline(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr)
{
	pfn_vkCmdTraceRaysKHR = (PFN_vkCmdTraceRaysKHR)getDeviceProcAddr(device, "vkCmdTraceRaysKHR");
	pfn_vkCreateRayTracingPipelinesKHR = (PFN_vkCreateRayTracingPipelinesKHR)getDeviceProcAddr(device, "vkCreateRayTracingPipelinesKHR");
	pfn_vkGetRayTracingShaderGroupHandlesKHR = (PFN_vkGetRayTracingShaderGroupHandlesKHR)getDeviceProcAddr(device, "vkGetRayTracingShaderGroupHandlesKHR");
	pfn_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR = (PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR)getDeviceProcAddr(device, "vkGetRayTracingCaptureReplayShaderGroupHandlesKHR");
	pfn_vkCmdTraceRaysIndirectKHR = (PFN_vkCmdTraceRaysIndirectKHR)getDeviceProcAddr(device, "vkCmdTraceRaysIndirectKHR");
	pfn_vkGetRayTracingShaderGroupStackSizeKHR = (PFN_vkGetRayTracingShaderGroupStackSizeKHR)getDeviceProcAddr(device, "vkGetRayTracingShaderGroupStackSizeKHR");
	pfn_vkCmdSetRayTracingPipelineStackSizeKHR = (PFN_vkCmdSetRayTracingPipelineStackSizeKHR)getDeviceProcAddr(device, "vkCmdSetRayTracingPipelineStackSizeKHR");
	int success = 1;
	success = success && (pfn_vkCmdTraceRaysKHR != 0);
	success = success && (pfn_vkCreateRayTracingPipelinesKHR != 0);
	success = success && (pfn_vkGetRayTracingShaderGroupHandlesKHR != 0);
	success = success && (pfn_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR != 0);
	success = success && (pfn_vkCmdTraceRaysIndirectKHR != 0);
	success = success && (pfn_vkGetRayTracingShaderGroupStackSizeKHR != 0);
	success = success && (pfn_vkCmdSetRayTracingPipelineStackSizeKHR != 0);
	has_VK_KHR_ray_tracing_pipeline = success;
	return success;
}
#endif

/* /////////////////////////////////// */
#if VK_NV_shading_rate_image
static PFN_vkCmdBindShadingRateImageNV pfn_vkCmdBindShadingRateImageNV = 0;
static PFN_vkCmdSetViewportShadingRatePaletteNV pfn_vkCmdSetViewportShadingRatePaletteNV = 0;
static PFN_vkCmdSetCoarseSampleOrderNV pfn_vkCmdSetCoarseSampleOrderNV = 0;

VKAPI_ATTR void VKAPI_CALL vkCmdBindShadingRateImageNV(
	VkCommandBuffer commandBuffer,
	VkImageView imageView,
	VkImageLayout imageLayout)
{
	assert(pfn_vkCmdBindShadingRateImageNV);
	pfn_vkCmdBindShadingRateImageNV(commandBuffer, imageView, imageLayout);
}
VKAPI_ATTR void VKAPI_CALL vkCmdSetViewportShadingRatePaletteNV(
	VkCommandBuffer commandBuffer,
	uint32_t firstViewport,
	uint32_t viewportCount,
	const VkShadingRatePaletteNV* pShadingRatePalettes)
{
	assert(pfn_vkCmdSetViewportShadingRatePaletteNV);
	pfn_vkCmdSetViewportShadingRatePaletteNV(commandBuffer, firstViewport, viewportCount, pShadingRatePalettes);
}
VKAPI_ATTR void VKAPI_CALL vkCmdSetCoarseSampleOrderNV(
	VkCommandBuffer commandBuffer,
	VkCoarseSampleOrderTypeNV sampleOrderType,
	uint32_t customSampleOrderCount,
	const VkCoarseSampleOrderCustomNV* pCustomSampleOrders)
{
	assert(pfn_vkCmdSetCoarseSampleOrderNV);
	pfn_vkCmdSetCoarseSampleOrderNV(commandBuffer, sampleOrderType, customSampleOrderCount, pCustomSampleOrders);
}

int has_VK_NV_shading_rate_image = 0;
int load_VK_NV_shading_rate_image(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr)
{
	pfn_vkCmdBindShadingRateImageNV = (PFN_vkCmdBindShadingRateImageNV)getDeviceProcAddr(device, "vkCmdBindShadingRateImageNV");
	pfn_vkCmdSetViewportShadingRatePaletteNV = (PFN_vkCmdSetViewportShadingRatePaletteNV)getDeviceProcAddr(device, "vkCmdSetViewportShadingRatePaletteNV");
	pfn_vkCmdSetCoarseSampleOrderNV = (PFN_vkCmdSetCoarseSampleOrderNV)getDeviceProcAddr(device, "vkCmdSetCoarseSampleOrderNV");
	int success = 1;
	success = success && (pfn_vkCmdBindShadingRateImageNV != 0);
	success = success && (pfn_vkCmdSetViewportShadingRatePaletteNV != 0);
	success = success && (pfn_vkCmdSetCoarseSampleOrderNV != 0);
	has_VK_NV_shading_rate_image = success;
	return success;
}
#endif

/* /////////////////////////////////// */
#if VK_NV_ray_tracing
static PFN_vkCreateAccelerationStructureNV pfn_vkCreateAccelerationStructureNV = 0;
static PFN_vkDestroyAccelerationStructureNV pfn_vkDestroyAccelerationStructureNV = 0;
static PFN_vkGetAccelerationStructureMemoryRequirementsNV pfn_vkGetAccelerationStructureMemoryRequirementsNV = 0;
static PFN_vkBindAccelerationStructureMemoryNV pfn_vkBindAccelerationStructureMemoryNV = 0;
static PFN_vkCmdBuildAccelerationStructureNV pfn_vkCmdBuildAccelerationStructureNV = 0;
static PFN_vkCmdCopyAccelerationStructureNV pfn_vkCmdCopyAccelerationStructureNV = 0;
static PFN_vkCmdTraceRaysNV pfn_vkCmdTraceRaysNV = 0;
static PFN_vkCreateRayTracingPipelinesNV pfn_vkCreateRayTracingPipelinesNV = 0;
static PFN_vkGetRayTracingShaderGroupHandlesNV pfn_vkGetRayTracingShaderGroupHandlesNV = 0;
static PFN_vkGetAccelerationStructureHandleNV pfn_vkGetAccelerationStructureHandleNV = 0;
static PFN_vkCmdWriteAccelerationStructuresPropertiesNV pfn_vkCmdWriteAccelerationStructuresPropertiesNV = 0;
static PFN_vkCompileDeferredNV pfn_vkCompileDeferredNV = 0;

VKAPI_ATTR VkResult VKAPI_CALL vkCreateAccelerationStructureNV(
	VkDevice device,
	const VkAccelerationStructureCreateInfoNV* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkAccelerationStructureNV* pAccelerationStructure)
{
	assert(pfn_vkCreateAccelerationStructureNV);
	return pfn_vkCreateAccelerationStructureNV(device, pCreateInfo, pAllocator, pAccelerationStructure);
}
VKAPI_ATTR void VKAPI_CALL vkDestroyAccelerationStructureNV(
	VkDevice device,
	VkAccelerationStructureNV accelerationStructure,
	const VkAllocationCallbacks* pAllocator)
{
	assert(pfn_vkDestroyAccelerationStructureNV);
	pfn_vkDestroyAccelerationStructureNV(device, accelerationStructure, pAllocator);
}
VKAPI_ATTR void VKAPI_CALL vkGetAccelerationStructureMemoryRequirementsNV(
	VkDevice device,
	const VkAccelerationStructureMemoryRequirementsInfoNV* pInfo,
	VkMemoryRequirements2KHR* pMemoryRequirements)
{
	assert(pfn_vkGetAccelerationStructureMemoryRequirementsNV);
	pfn_vkGetAccelerationStructureMemoryRequirementsNV(device, pInfo, pMemoryRequirements);
}
VKAPI_ATTR VkResult VKAPI_CALL vkBindAccelerationStructureMemoryNV(
	VkDevice device,
	uint32_t bindInfoCount,
	const VkBindAccelerationStructureMemoryInfoNV* pBindInfos)
{
	assert(pfn_vkBindAccelerationStructureMemoryNV);
	return pfn_vkBindAccelerationStructureMemoryNV(device, bindInfoCount, pBindInfos);
}
VKAPI_ATTR void VKAPI_CALL vkCmdBuildAccelerationStructureNV(
	VkCommandBuffer commandBuffer,
	const VkAccelerationStructureInfoNV* pInfo,
	VkBuffer instanceData,
	VkDeviceSize instanceOffset,
	VkBool32 update,
	VkAccelerationStructureNV dst,
	VkAccelerationStructureNV src,
	VkBuffer scratch,
	VkDeviceSize scratchOffset)
{
	assert(pfn_vkCmdBuildAccelerationStructureNV);
	pfn_vkCmdBuildAccelerationStructureNV(commandBuffer, pInfo, instanceData, instanceOffset, update, dst, src, scratch, scratchOffset);
}
VKAPI_ATTR void VKAPI_CALL vkCmdCopyAccelerationStructureNV(
	VkCommandBuffer commandBuffer,
	VkAccelerationStructureNV dst,
	VkAccelerationStructureNV src,
	VkCopyAccelerationStructureModeNV mode)
{
	assert(pfn_vkCmdCopyAccelerationStructureNV);
	pfn_vkCmdCopyAccelerationStructureNV(commandBuffer, dst, src, mode);
}
VKAPI_ATTR void VKAPI_CALL vkCmdTraceRaysNV(
	VkCommandBuffer commandBuffer,
	VkBuffer raygenShaderBindingTableBuffer,
	VkDeviceSize raygenShaderBindingOffset,
	VkBuffer missShaderBindingTableBuffer,
	VkDeviceSize missShaderBindingOffset,
	VkDeviceSize missShaderBindingStride,
	VkBuffer hitShaderBindingTableBuffer,
	VkDeviceSize hitShaderBindingOffset,
	VkDeviceSize hitShaderBindingStride,
	VkBuffer callableShaderBindingTableBuffer,
	VkDeviceSize callableShaderBindingOffset,
	VkDeviceSize callableShaderBindingStride,
	uint32_t width,
	uint32_t height,
	uint32_t depth)
{
	assert(pfn_vkCmdTraceRaysNV);
	pfn_vkCmdTraceRaysNV(commandBuffer, raygenShaderBindingTableBuffer, raygenShaderBindingOffset, missShaderBindingTableBuffer, missShaderBindingOffset, missShaderBindingStride, hitShaderBindingTableBuffer, hitShaderBindingOffset, hitShaderBindingStride, callableShaderBindingTableBuffer, callableShaderBindingOffset, callableShaderBindingStride, width, height, depth);
}
VKAPI_ATTR VkResult VKAPI_CALL vkCreateRayTracingPipelinesNV(
	VkDevice device,
	VkPipelineCache pipelineCache,
	uint32_t createInfoCount,
	const VkRayTracingPipelineCreateInfoNV* pCreateInfos,
	const VkAllocationCallbacks* pAllocator,
	VkPipeline* pPipelines)
{
	assert(pfn_vkCreateRayTracingPipelinesNV);
	return pfn_vkCreateRayTracingPipelinesNV(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
}
VKAPI_ATTR VkResult VKAPI_CALL vkGetRayTracingShaderGroupHandlesNV(
	VkDevice device,
	VkPipeline pipeline,
	uint32_t firstGroup,
	uint32_t groupCount,
	size_t dataSize,
	void* pData)
{
	assert(pfn_vkGetRayTracingShaderGroupHandlesNV);
	return pfn_vkGetRayTracingShaderGroupHandlesNV(device, pipeline, firstGroup, groupCount, dataSize, pData);
}
VKAPI_ATTR VkResult VKAPI_CALL vkGetAccelerationStructureHandleNV(
	VkDevice device,
	VkAccelerationStructureNV accelerationStructure,
	size_t dataSize,
	void* pData)
{
	assert(pfn_vkGetAccelerationStructureHandleNV);
	return pfn_vkGetAccelerationStructureHandleNV(device, accelerationStructure, dataSize, pData);
}
VKAPI_ATTR void VKAPI_CALL vkCmdWriteAccelerationStructuresPropertiesNV(
	VkCommandBuffer commandBuffer,
	uint32_t accelerationStructureCount,
	const VkAccelerationStructureNV* pAccelerationStructures,
	VkQueryType queryType,
	VkQueryPool queryPool,
	uint32_t firstQuery)
{
	assert(pfn_vkCmdWriteAccelerationStructuresPropertiesNV);
	pfn_vkCmdWriteAccelerationStructuresPropertiesNV(commandBuffer, accelerationStructureCount, pAccelerationStructures, queryType, queryPool, firstQuery);
}
VKAPI_ATTR VkResult VKAPI_CALL vkCompileDeferredNV(
	VkDevice device,
	VkPipeline pipeline,
	uint32_t shader)
{
	assert(pfn_vkCompileDeferredNV);
	return pfn_vkCompileDeferredNV(device, pipeline, shader);
}

int has_VK_NV_ray_tracing = 0;
int load_VK_NV_ray_tracing(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr)
{
	pfn_vkCreateAccelerationStructureNV = (PFN_vkCreateAccelerationStructureNV)getDeviceProcAddr(device, "vkCreateAccelerationStructureNV");
	pfn_vkDestroyAccelerationStructureNV = (PFN_vkDestroyAccelerationStructureNV)getDeviceProcAddr(device, "vkDestroyAccelerationStructureNV");
	pfn_vkGetAccelerationStructureMemoryRequirementsNV = (PFN_vkGetAccelerationStructureMemoryRequirementsNV)getDeviceProcAddr(device, "vkGetAccelerationStructureMemoryRequirementsNV");
	pfn_vkBindAccelerationStructureMemoryNV = (PFN_vkBindAccelerationStructureMemoryNV)getDeviceProcAddr(device, "vkBindAccelerationStructureMemoryNV");
	pfn_vkCmdBuildAccelerationStructureNV = (PFN_vkCmdBuildAccelerationStructureNV)getDeviceProcAddr(device, "vkCmdBuildAccelerationStructureNV");
	pfn_vkCmdCopyAccelerationStructureNV = (PFN_vkCmdCopyAccelerationStructureNV)getDeviceProcAddr(device, "vkCmdCopyAccelerationStructureNV");
	pfn_vkCmdTraceRaysNV = (PFN_vkCmdTraceRaysNV)getDeviceProcAddr(device, "vkCmdTraceRaysNV");
	pfn_vkCreateRayTracingPipelinesNV = (PFN_vkCreateRayTracingPipelinesNV)getDeviceProcAddr(device, "vkCreateRayTracingPipelinesNV");
	pfn_vkGetRayTracingShaderGroupHandlesNV = (PFN_vkGetRayTracingShaderGroupHandlesNV)getDeviceProcAddr(device, "vkGetRayTracingShaderGroupHandlesNV");
	pfn_vkGetAccelerationStructureHandleNV = (PFN_vkGetAccelerationStructureHandleNV)getDeviceProcAddr(device, "vkGetAccelerationStructureHandleNV");
	pfn_vkCmdWriteAccelerationStructuresPropertiesNV = (PFN_vkCmdWriteAccelerationStructuresPropertiesNV)getDeviceProcAddr(device, "vkCmdWriteAccelerationStructuresPropertiesNV");
	pfn_vkCompileDeferredNV = (PFN_vkCompileDeferredNV)getDeviceProcAddr(device, "vkCompileDeferredNV");
	int success = 1;
	success = success && (pfn_vkCreateAccelerationStructureNV != 0);
	success = success && (pfn_vkDestroyAccelerationStructureNV != 0);
	success = success && (pfn_vkGetAccelerationStructureMemoryRequirementsNV != 0);
	success = success && (pfn_vkBindAccelerationStructureMemoryNV != 0);
	success = success && (pfn_vkCmdBuildAccelerationStructureNV != 0);
	success = success && (pfn_vkCmdCopyAccelerationStructureNV != 0);
	success = success && (pfn_vkCmdTraceRaysNV != 0);
	success = success && (pfn_vkCreateRayTracingPipelinesNV != 0);
	success = success && (pfn_vkGetRayTracingShaderGroupHandlesNV != 0);
	success = success && (pfn_vkGetAccelerationStructureHandleNV != 0);
	success = success && (pfn_vkCmdWriteAccelerationStructuresPropertiesNV != 0);
	success = success && (pfn_vkCompileDeferredNV != 0);
	has_VK_NV_ray_tracing = success;
	return success;
}
#endif

/* /////////////////////////////////// */
#if VK_KHR_draw_indirect_count
static PFN_vkCmdDrawIndirectCountKHR pfn_vkCmdDrawIndirectCountKHR = 0;
static PFN_vkCmdDrawIndexedIndirectCountKHR pfn_vkCmdDrawIndexedIndirectCountKHR = 0;

VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndirectCountKHR(
	VkCommandBuffer commandBuffer,
	VkBuffer buffer,
	VkDeviceSize offset,
	VkBuffer countBuffer,
	VkDeviceSize countBufferOffset,
	uint32_t maxDrawCount,
	uint32_t stride)
{
	assert(pfn_vkCmdDrawIndirectCountKHR);
	pfn_vkCmdDrawIndirectCountKHR(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
}
VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndexedIndirectCountKHR(
	VkCommandBuffer commandBuffer,
	VkBuffer buffer,
	VkDeviceSize offset,
	VkBuffer countBuffer,
	VkDeviceSize countBufferOffset,
	uint32_t maxDrawCount,
	uint32_t stride)
{
	assert(pfn_vkCmdDrawIndexedIndirectCountKHR);
	pfn_vkCmdDrawIndexedIndirectCountKHR(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
}

int has_VK_KHR_draw_indirect_count = 0;
int load_VK_KHR_draw_indirect_count(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr)
{
	pfn_vkCmdDrawIndirectCountKHR = (PFN_vkCmdDrawIndirectCountKHR)getDeviceProcAddr(device, "vkCmdDrawIndirectCountKHR");
	pfn_vkCmdDrawIndexedIndirectCountKHR = (PFN_vkCmdDrawIndexedIndirectCountKHR)getDeviceProcAddr(device, "vkCmdDrawIndexedIndirectCountKHR");
	int success = 1;
	success = success && (pfn_vkCmdDrawIndirectCountKHR != 0);
	success = success && (pfn_vkCmdDrawIndexedIndirectCountKHR != 0);
	has_VK_KHR_draw_indirect_count = success;
	return success;
}
#endif

/* /////////////////////////////////// */
#if VK_EXT_calibrated_timestamps
static PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT pfn_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT = 0;
static PFN_vkGetCalibratedTimestampsEXT pfn_vkGetCalibratedTimestampsEXT = 0;

VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceCalibrateableTimeDomainsEXT(
	VkPhysicalDevice physicalDevice,
	uint32_t* pTimeDomainCount,
	VkTimeDomainEXT* pTimeDomains)
{
	assert(pfn_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT);
	return pfn_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT(physicalDevice, pTimeDomainCount, pTimeDomains);
}
VKAPI_ATTR VkResult VKAPI_CALL vkGetCalibratedTimestampsEXT(
	VkDevice device,
	uint32_t timestampCount,
	const VkCalibratedTimestampInfoEXT* pTimestampInfos,
	uint64_t* pTimestamps,
	uint64_t* pMaxDeviation)
{
	assert(pfn_vkGetCalibratedTimestampsEXT);
	return pfn_vkGetCalibratedTimestampsEXT(device, timestampCount, pTimestampInfos, pTimestamps, pMaxDeviation);
}

int has_VK_EXT_calibrated_timestamps = 0;
int load_VK_EXT_calibrated_timestamps(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr)
{
	pfn_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT = (PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT)getDeviceProcAddr(device, "vkGetPhysicalDeviceCalibrateableTimeDomainsEXT");
	pfn_vkGetCalibratedTimestampsEXT = (PFN_vkGetCalibratedTimestampsEXT)getDeviceProcAddr(device, "vkGetCalibratedTimestampsEXT");
	int success = 1;
	success = success && (pfn_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT != 0);
	success = success && (pfn_vkGetCalibratedTimestampsEXT != 0);
	has_VK_EXT_calibrated_timestamps = success;
	return success;
}
#endif

/* /////////////////////////////////// */
#if VK_NV_mesh_shader
static PFN_vkCmdDrawMeshTasksNV pfn_vkCmdDrawMeshTasksNV = 0;
static PFN_vkCmdDrawMeshTasksIndirectNV pfn_vkCmdDrawMeshTasksIndirectNV = 0;
static PFN_vkCmdDrawMeshTasksIndirectCountNV pfn_vkCmdDrawMeshTasksIndirectCountNV = 0;

VKAPI_ATTR void VKAPI_CALL vkCmdDrawMeshTasksNV(
	VkCommandBuffer commandBuffer,
	uint32_t taskCount,
	uint32_t firstTask)
{
	assert(pfn_vkCmdDrawMeshTasksNV);
	pfn_vkCmdDrawMeshTasksNV(commandBuffer, taskCount, firstTask);
}
VKAPI_ATTR void VKAPI_CALL vkCmdDrawMeshTasksIndirectNV(
	VkCommandBuffer commandBuffer,
	VkBuffer buffer,
	VkDeviceSize offset,
	uint32_t drawCount,
	uint32_t stride)
{
	assert(pfn_vkCmdDrawMeshTasksIndirectNV);
	pfn_vkCmdDrawMeshTasksIndirectNV(commandBuffer, buffer, offset, drawCount, stride);
}
VKAPI_ATTR void VKAPI_CALL vkCmdDrawMeshTasksIndirectCountNV(
	VkCommandBuffer commandBuffer,
	VkBuffer buffer,
	VkDeviceSize offset,
	VkBuffer countBuffer,
	VkDeviceSize countBufferOffset,
	uint32_t maxDrawCount,
	uint32_t stride)
{
	assert(pfn_vkCmdDrawMeshTasksIndirectCountNV);
	pfn_vkCmdDrawMeshTasksIndirectCountNV(commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride);
}

int has_VK_NV_mesh_shader = 0;
int load_VK_NV_mesh_shader(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr)
{
	pfn_vkCmdDrawMeshTasksNV = (PFN_vkCmdDrawMeshTasksNV)getDeviceProcAddr(device, "vkCmdDrawMeshTasksNV");
	pfn_vkCmdDrawMeshTasksIndirectNV = (PFN_vkCmdDrawMeshTasksIndirectNV)getDeviceProcAddr(device, "vkCmdDrawMeshTasksIndirectNV");
	pfn_vkCmdDrawMeshTasksIndirectCountNV = (PFN_vkCmdDrawMeshTasksIndirectCountNV)getDeviceProcAddr(device, "vkCmdDrawMeshTasksIndirectCountNV");
	int success = 1;
	success = success && (pfn_vkCmdDrawMeshTasksNV != 0);
	success = success && (pfn_vkCmdDrawMeshTasksIndirectNV != 0);
	success = success && (pfn_vkCmdDrawMeshTasksIndirectCountNV != 0);
	has_VK_NV_mesh_shader = success;
	return success;
}
#endif

/* /////////////////////////////////// */
#if VK_NV_scissor_exclusive
static PFN_vkCmdSetExclusiveScissorNV pfn_vkCmdSetExclusiveScissorNV = 0;

VKAPI_ATTR void VKAPI_CALL vkCmdSetExclusiveScissorNV(
	VkCommandBuffer commandBuffer,
	uint32_t firstExclusiveScissor,
	uint32_t exclusiveScissorCount,
	const VkRect2D* pExclusiveScissors)
{
	assert(pfn_vkCmdSetExclusiveScissorNV);
	pfn_vkCmdSetExclusiveScissorNV(commandBuffer, firstExclusiveScissor, exclusiveScissorCount, pExclusiveScissors);
}

int has_VK_NV_scissor_exclusive = 0;
int load_VK_NV_scissor_exclusive(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr)
{
	pfn_vkCmdSetExclusiveScissorNV = (PFN_vkCmdSetExclusiveScissorNV)getDeviceProcAddr(device, "vkCmdSetExclusiveScissorNV");
	int success = 1;
	success = success && (pfn_vkCmdSetExclusiveScissorNV != 0);
	has_VK_NV_scissor_exclusive = success;
	return success;
}
#endif

/* /////////////////////////////////// */
#if VK_EXT_buffer_device_address
static PFN_vkGetBufferDeviceAddressEXT pfn_vkGetBufferDeviceAddressEXT = 0;

VKAPI_ATTR VkDeviceAddress VKAPI_CALL vkGetBufferDeviceAddressEXT(
	VkDevice device,
	const VkBufferDeviceAddressInfoEXT* pInfo)
{
	assert(pfn_vkGetBufferDeviceAddressEXT);
	return pfn_vkGetBufferDeviceAddressEXT(device, pInfo);
}

int has_VK_EXT_buffer_device_address = 0;
int load_VK_EXT_buffer_device_address(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr)
{
	pfn_vkGetBufferDeviceAddressEXT = (PFN_vkGetBufferDeviceAddressEXT)getDeviceProcAddr(device, "vkGetBufferDeviceAddressEXT");
	int success = 1;
	success = success && (pfn_vkGetBufferDeviceAddressEXT != 0);
	has_VK_EXT_buffer_device_address = success;
	return success;
}
#endif

/* /////////////////////////////////// */
#if VK_NV_cooperative_matrix
static PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesNV pfn_vkGetPhysicalDeviceCooperativeMatrixPropertiesNV = 0;

VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceCooperativeMatrixPropertiesNV(
	VkPhysicalDevice physicalDevice,
	uint32_t* pPropertyCount,
	VkCooperativeMatrixPropertiesNV* pProperties)
{
	assert(pfn_vkGetPhysicalDeviceCooperativeMatrixPropertiesNV);
	return pfn_vkGetPhysicalDeviceCooperativeMatrixPropertiesNV(physicalDevice, pPropertyCount, pProperties);
}

int has_VK_NV_cooperative_matrix = 0;
int load_VK_NV_cooperative_matrix(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr)
{
	pfn_vkGetPhysicalDeviceCooperativeMatrixPropertiesNV = (PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesNV)getDeviceProcAddr(device, "vkGetPhysicalDeviceCooperativeMatrixPropertiesNV");
	int success = 1;
	success = success && (pfn_vkGetPhysicalDeviceCooperativeMatrixPropertiesNV != 0);
	has_VK_NV_cooperative_matrix = success;
	return success;
}
#endif

/* /////////////////////////////////// */
#if VK_NV_coverage_reduction_mode
static PFN_vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV pfn_vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV = 0;

VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV(
	VkPhysicalDevice physicalDevice,
	uint32_t* pCombinationCount,
	VkFramebufferMixedSamplesCombinationNV* pCombinations)
{
	assert(pfn_vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV);
	return pfn_vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV(physicalDevice, pCombinationCount, pCombinations);
}

int has_VK_NV_coverage_reduction_mode = 0;
int load_VK_NV_coverage_reduction_mode(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr)
{
	pfn_vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV = (PFN_vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV)getDeviceProcAddr(device, "vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV");
	int success = 1;
	success = success && (pfn_vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV != 0);
	has_VK_NV_coverage_reduction_mode = success;
	return success;
}
#endif

/* /////////////////////////////////// */
#if VK_KHR_buffer_device_address
static PFN_vkGetBufferDeviceAddressKHR pfn_vkGetBufferDeviceAddressKHR = 0;
static PFN_vkGetBufferOpaqueCaptureAddressKHR pfn_vkGetBufferOpaqueCaptureAddressKHR = 0;
static PFN_vkGetDeviceMemoryOpaqueCaptureAddressKHR pfn_vkGetDeviceMemoryOpaqueCaptureAddressKHR = 0;

VKAPI_ATTR VkDeviceAddress VKAPI_CALL vkGetBufferDeviceAddressKHR(
	VkDevice device,
	const VkBufferDeviceAddressInfoKHR* pInfo)
{
	assert(pfn_vkGetBufferDeviceAddressKHR);
	return pfn_vkGetBufferDeviceAddressKHR(device, pInfo);
}
VKAPI_ATTR uint64_t VKAPI_CALL vkGetBufferOpaqueCaptureAddressKHR(
	VkDevice device,
	const VkBufferDeviceAddressInfoKHR* pInfo)
{
	assert(pfn_vkGetBufferOpaqueCaptureAddressKHR);
	return pfn_vkGetBufferOpaqueCaptureAddressKHR(device, pInfo);
}
VKAPI_ATTR uint64_t VKAPI_CALL vkGetDeviceMemoryOpaqueCaptureAddressKHR(
	VkDevice device,
	const VkDeviceMemoryOpaqueCaptureAddressInfoKHR* pInfo)
{
	assert(pfn_vkGetDeviceMemoryOpaqueCaptureAddressKHR);
	return pfn_vkGetDeviceMemoryOpaqueCaptureAddressKHR(device, pInfo);
}

int has_VK_KHR_buffer_device_address = 0;
int load_VK_KHR_buffer_device_address(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr)
{
	pfn_vkGetBufferDeviceAddressKHR = (PFN_vkGetBufferDeviceAddressKHR)getDeviceProcAddr(device, "vkGetBufferDeviceAddressKHR");
	pfn_vkGetBufferOpaqueCaptureAddressKHR = (PFN_vkGetBufferOpaqueCaptureAddressKHR)getDeviceProcAddr(device, "vkGetBufferOpaqueCaptureAddressKHR");
	pfn_vkGetDeviceMemoryOpaqueCaptureAddressKHR = (PFN_vkGetDeviceMemoryOpaqueCaptureAddressKHR)getDeviceProcAddr(device, "vkGetDeviceMemoryOpaqueCaptureAddressKHR");
	int success = 1;
	success = success && (pfn_vkGetBufferDeviceAddressKHR != 0);
	success = success && (pfn_vkGetBufferOpaqueCaptureAddressKHR != 0);
	success = success && (pfn_vkGetDeviceMemoryOpaqueCaptureAddressKHR != 0);
	has_VK_KHR_buffer_device_address = success;
	return success;
}
#endif

/* /////////////////////////////////// */
#if VK_EXT_host_query_reset
static PFN_vkResetQueryPoolEXT pfn_vkResetQueryPoolEXT = 0;

VKAPI_ATTR void VKAPI_CALL vkResetQueryPoolEXT(
	VkDevice device,
	VkQueryPool queryPool,
	uint32_t firstQuery,
	uint32_t queryCount)
{
	assert(pfn_vkResetQueryPoolEXT);
	pfn_vkResetQueryPoolEXT(device, queryPool, firstQuery, queryCount);
}

int has_VK_EXT_host_query_reset = 0;
int load_VK_EXT_host_query_reset(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr)
{
	pfn_vkResetQueryPoolEXT = (PFN_vkResetQueryPoolEXT)getDeviceProcAddr(device, "vkResetQueryPoolEXT");
	int success = 1;
	success = success && (pfn_vkResetQueryPoolEXT != 0);
	has_VK_EXT_host_query_reset = success;
	return success;
}
#endif

/* /////////////////////////////////// */
#if VK_KHR_pipeline_executable_properties
static PFN_vkGetPipelineExecutablePropertiesKHR pfn_vkGetPipelineExecutablePropertiesKHR = 0;
static PFN_vkGetPipelineExecutableStatisticsKHR pfn_vkGetPipelineExecutableStatisticsKHR = 0;
static PFN_vkGetPipelineExecutableInternalRepresentationsKHR pfn_vkGetPipelineExecutableInternalRepresentationsKHR = 0;

VKAPI_ATTR VkResult VKAPI_CALL vkGetPipelineExecutablePropertiesKHR(
	VkDevice device,
	const VkPipelineInfoKHR* pPipelineInfo,
	uint32_t* pExecutableCount,
	VkPipelineExecutablePropertiesKHR* pProperties)
{
	assert(pfn_vkGetPipelineExecutablePropertiesKHR);
	return pfn_vkGetPipelineExecutablePropertiesKHR(device, pPipelineInfo, pExecutableCount, pProperties);
}
VKAPI_ATTR VkResult VKAPI_CALL vkGetPipelineExecutableStatisticsKHR(
	VkDevice device,
	const VkPipelineExecutableInfoKHR* pExecutableInfo,
	uint32_t* pStatisticCount,
	VkPipelineExecutableStatisticKHR* pStatistics)
{
	assert(pfn_vkGetPipelineExecutableStatisticsKHR);
	return pfn_vkGetPipelineExecutableStatisticsKHR(device, pExecutableInfo, pStatisticCount, pStatistics);
}
VKAPI_ATTR VkResult VKAPI_CALL vkGetPipelineExecutableInternalRepresentationsKHR(
	VkDevice device,
	const VkPipelineExecutableInfoKHR* pExecutableInfo,
	uint32_t* pInternalRepresentationCount,
	VkPipelineExecutableInternalRepresentationKHR* pInternalRepresentations)
{
	assert(pfn_vkGetPipelineExecutableInternalRepresentationsKHR);
	return pfn_vkGetPipelineExecutableInternalRepresentationsKHR(device, pExecutableInfo, pInternalRepresentationCount, pInternalRepresentations);
}

int has_VK_KHR_pipeline_executable_properties = 0;
int load_VK_KHR_pipeline_executable_properties(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr)
{
	pfn_vkGetPipelineExecutablePropertiesKHR = (PFN_vkGetPipelineExecutablePropertiesKHR)getDeviceProcAddr(device, "vkGetPipelineExecutablePropertiesKHR");
	pfn_vkGetPipelineExecutableStatisticsKHR = (PFN_vkGetPipelineExecutableStatisticsKHR)getDeviceProcAddr(device, "vkGetPipelineExecutableStatisticsKHR");
	pfn_vkGetPipelineExecutableInternalRepresentationsKHR = (PFN_vkGetPipelineExecutableInternalRepresentationsKHR)getDeviceProcAddr(device, "vkGetPipelineExecutableInternalRepresentationsKHR");
	int success = 1;
	success = success && (pfn_vkGetPipelineExecutablePropertiesKHR != 0);
	success = success && (pfn_vkGetPipelineExecutableStatisticsKHR != 0);
	success = success && (pfn_vkGetPipelineExecutableInternalRepresentationsKHR != 0);
	has_VK_KHR_pipeline_executable_properties = success;
	return success;
}
#endif

/* /////////////////////////////////// */
#if VK_NV_device_generated_commands
static PFN_vkGetGeneratedCommandsMemoryRequirementsNV pfn_vkGetGeneratedCommandsMemoryRequirementsNV = 0;
static PFN_vkCmdPreprocessGeneratedCommandsNV pfn_vkCmdPreprocessGeneratedCommandsNV = 0;
static PFN_vkCmdExecuteGeneratedCommandsNV pfn_vkCmdExecuteGeneratedCommandsNV = 0;
static PFN_vkCmdBindPipelineShaderGroupNV pfn_vkCmdBindPipelineShaderGroupNV = 0;
static PFN_vkCreateIndirectCommandsLayoutNV pfn_vkCreateIndirectCommandsLayoutNV = 0;
static PFN_vkDestroyIndirectCommandsLayoutNV pfn_vkDestroyIndirectCommandsLayoutNV = 0;

VKAPI_ATTR void VKAPI_CALL vkGetGeneratedCommandsMemoryRequirementsNV(
	VkDevice device,
	const VkGeneratedCommandsMemoryRequirementsInfoNV* pInfo,
	VkMemoryRequirements2* pMemoryRequirements)
{
	assert(pfn_vkGetGeneratedCommandsMemoryRequirementsNV);
	pfn_vkGetGeneratedCommandsMemoryRequirementsNV(device, pInfo, pMemoryRequirements);
}
VKAPI_ATTR void VKAPI_CALL vkCmdPreprocessGeneratedCommandsNV(
	VkCommandBuffer commandBuffer,
	const VkGeneratedCommandsInfoNV* pGeneratedCommandsInfo)
{
	assert(pfn_vkCmdPreprocessGeneratedCommandsNV);
	pfn_vkCmdPreprocessGeneratedCommandsNV(commandBuffer, pGeneratedCommandsInfo);
}
VKAPI_ATTR void VKAPI_CALL vkCmdExecuteGeneratedCommandsNV(
	VkCommandBuffer commandBuffer,
	VkBool32 isPreprocessed,
	const VkGeneratedCommandsInfoNV* pGeneratedCommandsInfo)
{
	assert(pfn_vkCmdExecuteGeneratedCommandsNV);
	pfn_vkCmdExecuteGeneratedCommandsNV(commandBuffer, isPreprocessed, pGeneratedCommandsInfo);
}
VKAPI_ATTR void VKAPI_CALL vkCmdBindPipelineShaderGroupNV(
	VkCommandBuffer commandBuffer,
	VkPipelineBindPoint pipelineBindPoint,
	VkPipeline pipeline,
	uint32_t groupIndex)
{
	assert(pfn_vkCmdBindPipelineShaderGroupNV);
	pfn_vkCmdBindPipelineShaderGroupNV(commandBuffer, pipelineBindPoint, pipeline, groupIndex);
}
VKAPI_ATTR VkResult VKAPI_CALL vkCreateIndirectCommandsLayoutNV(
	VkDevice device,
	const VkIndirectCommandsLayoutCreateInfoNV* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkIndirectCommandsLayoutNV* pIndirectCommandsLayout)
{
	assert(pfn_vkCreateIndirectCommandsLayoutNV);
	return pfn_vkCreateIndirectCommandsLayoutNV(device, pCreateInfo, pAllocator, pIndirectCommandsLayout);
}
VKAPI_ATTR void VKAPI_CALL vkDestroyIndirectCommandsLayoutNV(
	VkDevice device,
	VkIndirectCommandsLayoutNV indirectCommandsLayout,
	const VkAllocationCallbacks* pAllocator)
{
	assert(pfn_vkDestroyIndirectCommandsLayoutNV);
	pfn_vkDestroyIndirectCommandsLayoutNV(device, indirectCommandsLayout, pAllocator);
}

int has_VK_NV_device_generated_commands = 0;
int load_VK_NV_device_generated_commands(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr)
{
	pfn_vkGetGeneratedCommandsMemoryRequirementsNV = (PFN_vkGetGeneratedCommandsMemoryRequirementsNV)getDeviceProcAddr(device, "vkGetGeneratedCommandsMemoryRequirementsNV");
	pfn_vkCmdPreprocessGeneratedCommandsNV = (PFN_vkCmdPreprocessGeneratedCommandsNV)getDeviceProcAddr(device, "vkCmdPreprocessGeneratedCommandsNV");
	pfn_vkCmdExecuteGeneratedCommandsNV = (PFN_vkCmdExecuteGeneratedCommandsNV)getDeviceProcAddr(device, "vkCmdExecuteGeneratedCommandsNV");
	pfn_vkCmdBindPipelineShaderGroupNV = (PFN_vkCmdBindPipelineShaderGroupNV)getDeviceProcAddr(device, "vkCmdBindPipelineShaderGroupNV");
	pfn_vkCreateIndirectCommandsLayoutNV = (PFN_vkCreateIndirectCommandsLayoutNV)getDeviceProcAddr(device, "vkCreateIndirectCommandsLayoutNV");
	pfn_vkDestroyIndirectCommandsLayoutNV = (PFN_vkDestroyIndirectCommandsLayoutNV)getDeviceProcAddr(device, "vkDestroyIndirectCommandsLayoutNV");
	int success = 1;
	success = success && (pfn_vkGetGeneratedCommandsMemoryRequirementsNV != 0);
	success = success && (pfn_vkCmdPreprocessGeneratedCommandsNV != 0);
	success = success && (pfn_vkCmdExecuteGeneratedCommandsNV != 0);
	success = success && (pfn_vkCmdBindPipelineShaderGroupNV != 0);
	success = success && (pfn_vkCreateIndirectCommandsLayoutNV != 0);
	success = success && (pfn_vkDestroyIndirectCommandsLayoutNV != 0);
	has_VK_NV_device_generated_commands = success;
	return success;
}
#endif

/* super load/reset */
void load_VK_EXTENSION_SUBSET(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr)
{
#if VK_EXT_debug_marker
	load_VK_EXT_debug_marker(instance, getInstanceProcAddr, device, getDeviceProcAddr);
#endif
#if VK_KHR_external_memory_win32 && defined(VK_USE_PLATFORM_WIN32_KHR)
	load_VK_KHR_external_memory_win32(instance, getInstanceProcAddr, device, getDeviceProcAddr);
#endif
#if VK_KHR_external_memory_fd
	load_VK_KHR_external_memory_fd(instance, getInstanceProcAddr, device, getDeviceProcAddr);
#endif
#if VK_KHR_external_semaphore_win32 && defined(VK_USE_PLATFORM_WIN32_KHR)
	load_VK_KHR_external_semaphore_win32(instance, getInstanceProcAddr, device, getDeviceProcAddr);
#endif
#if VK_KHR_external_semaphore_fd
	load_VK_KHR_external_semaphore_fd(instance, getInstanceProcAddr, device, getDeviceProcAddr);
#endif
#if VK_KHR_push_descriptor
	load_VK_KHR_push_descriptor(instance, getInstanceProcAddr, device, getDeviceProcAddr);
#endif
#if VK_KHR_create_renderpass2
	load_VK_KHR_create_renderpass2(instance, getInstanceProcAddr, device, getDeviceProcAddr);
#endif
#if VK_KHR_external_fence_win32 && defined(VK_USE_PLATFORM_WIN32_KHR)
	load_VK_KHR_external_fence_win32(instance, getInstanceProcAddr, device, getDeviceProcAddr);
#endif
#if VK_EXT_debug_utils
	load_VK_EXT_debug_utils(instance, getInstanceProcAddr, device, getDeviceProcAddr);
#endif
#if VK_EXT_sample_locations
	load_VK_EXT_sample_locations(instance, getInstanceProcAddr, device, getDeviceProcAddr);
#endif
#if VK_KHR_acceleration_structure
	load_VK_KHR_acceleration_structure(instance, getInstanceProcAddr, device, getDeviceProcAddr);
#endif
#if VK_KHR_ray_tracing_pipeline
	load_VK_KHR_ray_tracing_pipeline(instance, getInstanceProcAddr, device, getDeviceProcAddr);
#endif
#if VK_NV_shading_rate_image
	load_VK_NV_shading_rate_image(instance, getInstanceProcAddr, device, getDeviceProcAddr);
#endif
#if VK_NV_ray_tracing
	load_VK_NV_ray_tracing(instance, getInstanceProcAddr, device, getDeviceProcAddr);
#endif
#if VK_KHR_draw_indirect_count
	load_VK_KHR_draw_indirect_count(instance, getInstanceProcAddr, device, getDeviceProcAddr);
#endif
#if VK_EXT_calibrated_timestamps
	load_VK_EXT_calibrated_timestamps(instance, getInstanceProcAddr, device, getDeviceProcAddr);
#endif
#if VK_NV_mesh_shader
	load_VK_NV_mesh_shader(instance, getInstanceProcAddr, device, getDeviceProcAddr);
#endif
#if VK_NV_scissor_exclusive
	load_VK_NV_scissor_exclusive(instance, getInstanceProcAddr, device, getDeviceProcAddr);
#endif
#if VK_EXT_buffer_device_address
	load_VK_EXT_buffer_device_address(instance, getInstanceProcAddr, device, getDeviceProcAddr);
#endif
#if VK_NV_cooperative_matrix
	load_VK_NV_cooperative_matrix(instance, getInstanceProcAddr, device, getDeviceProcAddr);
#endif
#if VK_NV_coverage_reduction_mode
	load_VK_NV_coverage_reduction_mode(instance, getInstanceProcAddr, device, getDeviceProcAddr);
#endif
#if VK_KHR_buffer_device_address
	load_VK_KHR_buffer_device_address(instance, getInstanceProcAddr, device, getDeviceProcAddr);
#endif
#if VK_EXT_host_query_reset
	load_VK_EXT_host_query_reset(instance, getInstanceProcAddr, device, getDeviceProcAddr);
#endif
#if VK_KHR_pipeline_executable_properties
	load_VK_KHR_pipeline_executable_properties(instance, getInstanceProcAddr, device, getDeviceProcAddr);
#endif
#if VK_NV_device_generated_commands
	load_VK_NV_device_generated_commands(instance, getInstanceProcAddr, device, getDeviceProcAddr);
#endif
}
void reset_VK_EXTENSION_SUBSET()
{
#if VK_EXT_debug_marker
	has_VK_EXT_debug_marker = 0;
	PFN_vkDebugMarkerSetObjectTagEXT pfn_vkDebugMarkerSetObjectTagEXT = 0;
	PFN_vkDebugMarkerSetObjectNameEXT pfn_vkDebugMarkerSetObjectNameEXT = 0;
	PFN_vkCmdDebugMarkerBeginEXT pfn_vkCmdDebugMarkerBeginEXT = 0;
	PFN_vkCmdDebugMarkerEndEXT pfn_vkCmdDebugMarkerEndEXT = 0;
	PFN_vkCmdDebugMarkerInsertEXT pfn_vkCmdDebugMarkerInsertEXT = 0;
#endif

#if VK_KHR_external_memory_win32 && defined(VK_USE_PLATFORM_WIN32_KHR)
	has_VK_KHR_external_memory_win32 = 0;
	PFN_vkGetMemoryWin32HandleKHR pfn_vkGetMemoryWin32HandleKHR = 0;
	PFN_vkGetMemoryWin32HandlePropertiesKHR pfn_vkGetMemoryWin32HandlePropertiesKHR = 0;
#endif

#if VK_KHR_external_memory_fd
	has_VK_KHR_external_memory_fd = 0;
	PFN_vkGetMemoryFdKHR pfn_vkGetMemoryFdKHR = 0;
	PFN_vkGetMemoryFdPropertiesKHR pfn_vkGetMemoryFdPropertiesKHR = 0;
#endif

#if VK_KHR_external_semaphore_win32 && defined(VK_USE_PLATFORM_WIN32_KHR)
	has_VK_KHR_external_semaphore_win32 = 0;
	PFN_vkImportSemaphoreWin32HandleKHR pfn_vkImportSemaphoreWin32HandleKHR = 0;
	PFN_vkGetSemaphoreWin32HandleKHR pfn_vkGetSemaphoreWin32HandleKHR = 0;
#endif

#if VK_KHR_external_semaphore_fd
	has_VK_KHR_external_semaphore_fd = 0;
	PFN_vkImportSemaphoreFdKHR pfn_vkImportSemaphoreFdKHR = 0;
	PFN_vkGetSemaphoreFdKHR pfn_vkGetSemaphoreFdKHR = 0;
#endif

#if VK_KHR_push_descriptor
	has_VK_KHR_push_descriptor = 0;
	PFN_vkCmdPushDescriptorSetKHR pfn_vkCmdPushDescriptorSetKHR = 0;
	PFN_vkCmdPushDescriptorSetWithTemplateKHR pfn_vkCmdPushDescriptorSetWithTemplateKHR = 0;
#endif

#if VK_KHR_create_renderpass2
	has_VK_KHR_create_renderpass2 = 0;
	PFN_vkCreateRenderPass2KHR pfn_vkCreateRenderPass2KHR = 0;
	PFN_vkCmdBeginRenderPass2KHR pfn_vkCmdBeginRenderPass2KHR = 0;
	PFN_vkCmdNextSubpass2KHR pfn_vkCmdNextSubpass2KHR = 0;
	PFN_vkCmdEndRenderPass2KHR pfn_vkCmdEndRenderPass2KHR = 0;
#endif

#if VK_KHR_external_fence_win32 && defined(VK_USE_PLATFORM_WIN32_KHR)
	has_VK_KHR_external_fence_win32 = 0;
	PFN_vkImportFenceWin32HandleKHR pfn_vkImportFenceWin32HandleKHR = 0;
	PFN_vkGetFenceWin32HandleKHR pfn_vkGetFenceWin32HandleKHR = 0;
#endif

#if VK_EXT_debug_utils
	has_VK_EXT_debug_utils = 0;
	PFN_vkSetDebugUtilsObjectNameEXT pfn_vkSetDebugUtilsObjectNameEXT = 0;
	PFN_vkSetDebugUtilsObjectTagEXT pfn_vkSetDebugUtilsObjectTagEXT = 0;
	PFN_vkQueueBeginDebugUtilsLabelEXT pfn_vkQueueBeginDebugUtilsLabelEXT = 0;
	PFN_vkQueueEndDebugUtilsLabelEXT pfn_vkQueueEndDebugUtilsLabelEXT = 0;
	PFN_vkQueueInsertDebugUtilsLabelEXT pfn_vkQueueInsertDebugUtilsLabelEXT = 0;
	PFN_vkCmdBeginDebugUtilsLabelEXT pfn_vkCmdBeginDebugUtilsLabelEXT = 0;
	PFN_vkCmdEndDebugUtilsLabelEXT pfn_vkCmdEndDebugUtilsLabelEXT = 0;
	PFN_vkCmdInsertDebugUtilsLabelEXT pfn_vkCmdInsertDebugUtilsLabelEXT = 0;
	PFN_vkCreateDebugUtilsMessengerEXT pfn_vkCreateDebugUtilsMessengerEXT = 0;
	PFN_vkDestroyDebugUtilsMessengerEXT pfn_vkDestroyDebugUtilsMessengerEXT = 0;
	PFN_vkSubmitDebugUtilsMessageEXT pfn_vkSubmitDebugUtilsMessageEXT = 0;
#endif

#if VK_EXT_sample_locations
	has_VK_EXT_sample_locations = 0;
	PFN_vkCmdSetSampleLocationsEXT pfn_vkCmdSetSampleLocationsEXT = 0;
	PFN_vkGetPhysicalDeviceMultisamplePropertiesEXT pfn_vkGetPhysicalDeviceMultisamplePropertiesEXT = 0;
#endif

#if VK_KHR_acceleration_structure
	has_VK_KHR_acceleration_structure = 0;
	PFN_vkCreateAccelerationStructureKHR pfn_vkCreateAccelerationStructureKHR = 0;
	PFN_vkDestroyAccelerationStructureKHR pfn_vkDestroyAccelerationStructureKHR = 0;
	PFN_vkCmdBuildAccelerationStructuresKHR pfn_vkCmdBuildAccelerationStructuresKHR = 0;
	PFN_vkCmdBuildAccelerationStructuresIndirectKHR pfn_vkCmdBuildAccelerationStructuresIndirectKHR = 0;
	PFN_vkBuildAccelerationStructuresKHR pfn_vkBuildAccelerationStructuresKHR = 0;
	PFN_vkCopyAccelerationStructureKHR pfn_vkCopyAccelerationStructureKHR = 0;
	PFN_vkCopyAccelerationStructureToMemoryKHR pfn_vkCopyAccelerationStructureToMemoryKHR = 0;
	PFN_vkCopyMemoryToAccelerationStructureKHR pfn_vkCopyMemoryToAccelerationStructureKHR = 0;
	PFN_vkWriteAccelerationStructuresPropertiesKHR pfn_vkWriteAccelerationStructuresPropertiesKHR = 0;
	PFN_vkCmdCopyAccelerationStructureKHR pfn_vkCmdCopyAccelerationStructureKHR = 0;
	PFN_vkCmdCopyAccelerationStructureToMemoryKHR pfn_vkCmdCopyAccelerationStructureToMemoryKHR = 0;
	PFN_vkCmdCopyMemoryToAccelerationStructureKHR pfn_vkCmdCopyMemoryToAccelerationStructureKHR = 0;
	PFN_vkGetAccelerationStructureDeviceAddressKHR pfn_vkGetAccelerationStructureDeviceAddressKHR = 0;
	PFN_vkCmdWriteAccelerationStructuresPropertiesKHR pfn_vkCmdWriteAccelerationStructuresPropertiesKHR = 0;
	PFN_vkGetDeviceAccelerationStructureCompatibilityKHR pfn_vkGetDeviceAccelerationStructureCompatibilityKHR = 0;
	PFN_vkGetAccelerationStructureBuildSizesKHR pfn_vkGetAccelerationStructureBuildSizesKHR = 0;
#endif

#if VK_KHR_ray_tracing_pipeline
	has_VK_KHR_ray_tracing_pipeline = 0;
	PFN_vkCmdTraceRaysKHR pfn_vkCmdTraceRaysKHR = 0;
	PFN_vkCreateRayTracingPipelinesKHR pfn_vkCreateRayTracingPipelinesKHR = 0;
	PFN_vkGetRayTracingShaderGroupHandlesKHR pfn_vkGetRayTracingShaderGroupHandlesKHR = 0;
	PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR pfn_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR = 0;
	PFN_vkCmdTraceRaysIndirectKHR pfn_vkCmdTraceRaysIndirectKHR = 0;
	PFN_vkGetRayTracingShaderGroupStackSizeKHR pfn_vkGetRayTracingShaderGroupStackSizeKHR = 0;
	PFN_vkCmdSetRayTracingPipelineStackSizeKHR pfn_vkCmdSetRayTracingPipelineStackSizeKHR = 0;
#endif

#if VK_NV_shading_rate_image
	has_VK_NV_shading_rate_image = 0;
	PFN_vkCmdBindShadingRateImageNV pfn_vkCmdBindShadingRateImageNV = 0;
	PFN_vkCmdSetViewportShadingRatePaletteNV pfn_vkCmdSetViewportShadingRatePaletteNV = 0;
	PFN_vkCmdSetCoarseSampleOrderNV pfn_vkCmdSetCoarseSampleOrderNV = 0;
#endif

#if VK_NV_ray_tracing
	has_VK_NV_ray_tracing = 0;
	PFN_vkCreateAccelerationStructureNV pfn_vkCreateAccelerationStructureNV = 0;
	PFN_vkDestroyAccelerationStructureNV pfn_vkDestroyAccelerationStructureNV = 0;
	PFN_vkGetAccelerationStructureMemoryRequirementsNV pfn_vkGetAccelerationStructureMemoryRequirementsNV = 0;
	PFN_vkBindAccelerationStructureMemoryNV pfn_vkBindAccelerationStructureMemoryNV = 0;
	PFN_vkCmdBuildAccelerationStructureNV pfn_vkCmdBuildAccelerationStructureNV = 0;
	PFN_vkCmdCopyAccelerationStructureNV pfn_vkCmdCopyAccelerationStructureNV = 0;
	PFN_vkCmdTraceRaysNV pfn_vkCmdTraceRaysNV = 0;
	PFN_vkCreateRayTracingPipelinesNV pfn_vkCreateRayTracingPipelinesNV = 0;
	PFN_vkGetRayTracingShaderGroupHandlesNV pfn_vkGetRayTracingShaderGroupHandlesNV = 0;
	PFN_vkGetAccelerationStructureHandleNV pfn_vkGetAccelerationStructureHandleNV = 0;
	PFN_vkCmdWriteAccelerationStructuresPropertiesNV pfn_vkCmdWriteAccelerationStructuresPropertiesNV = 0;
	PFN_vkCompileDeferredNV pfn_vkCompileDeferredNV = 0;
#endif

#if VK_KHR_draw_indirect_count
	has_VK_KHR_draw_indirect_count = 0;
	PFN_vkCmdDrawIndirectCountKHR pfn_vkCmdDrawIndirectCountKHR = 0;
	PFN_vkCmdDrawIndexedIndirectCountKHR pfn_vkCmdDrawIndexedIndirectCountKHR = 0;
#endif

#if VK_EXT_calibrated_timestamps
	has_VK_EXT_calibrated_timestamps = 0;
	PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT pfn_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT = 0;
	PFN_vkGetCalibratedTimestampsEXT pfn_vkGetCalibratedTimestampsEXT = 0;
#endif

#if VK_NV_mesh_shader
	has_VK_NV_mesh_shader = 0;
	PFN_vkCmdDrawMeshTasksNV pfn_vkCmdDrawMeshTasksNV = 0;
	PFN_vkCmdDrawMeshTasksIndirectNV pfn_vkCmdDrawMeshTasksIndirectNV = 0;
	PFN_vkCmdDrawMeshTasksIndirectCountNV pfn_vkCmdDrawMeshTasksIndirectCountNV = 0;
#endif

#if VK_NV_scissor_exclusive
	has_VK_NV_scissor_exclusive = 0;
	PFN_vkCmdSetExclusiveScissorNV pfn_vkCmdSetExclusiveScissorNV = 0;
#endif

#if VK_EXT_buffer_device_address
	has_VK_EXT_buffer_device_address = 0;
	PFN_vkGetBufferDeviceAddressEXT pfn_vkGetBufferDeviceAddressEXT = 0;
#endif

#if VK_NV_cooperative_matrix
	has_VK_NV_cooperative_matrix = 0;
	PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesNV pfn_vkGetPhysicalDeviceCooperativeMatrixPropertiesNV = 0;
#endif

#if VK_NV_coverage_reduction_mode
	has_VK_NV_coverage_reduction_mode = 0;
	PFN_vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV pfn_vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV = 0;
#endif

#if VK_KHR_buffer_device_address
	has_VK_KHR_buffer_device_address = 0;
	PFN_vkGetBufferDeviceAddressKHR pfn_vkGetBufferDeviceAddressKHR = 0;
	PFN_vkGetBufferOpaqueCaptureAddressKHR pfn_vkGetBufferOpaqueCaptureAddressKHR = 0;
	PFN_vkGetDeviceMemoryOpaqueCaptureAddressKHR pfn_vkGetDeviceMemoryOpaqueCaptureAddressKHR = 0;
#endif

#if VK_EXT_host_query_reset
	has_VK_EXT_host_query_reset = 0;
	PFN_vkResetQueryPoolEXT pfn_vkResetQueryPoolEXT = 0;
#endif

#if VK_KHR_pipeline_executable_properties
	has_VK_KHR_pipeline_executable_properties = 0;
	PFN_vkGetPipelineExecutablePropertiesKHR pfn_vkGetPipelineExecutablePropertiesKHR = 0;
	PFN_vkGetPipelineExecutableStatisticsKHR pfn_vkGetPipelineExecutableStatisticsKHR = 0;
	PFN_vkGetPipelineExecutableInternalRepresentationsKHR pfn_vkGetPipelineExecutableInternalRepresentationsKHR = 0;
#endif

#if VK_NV_device_generated_commands
	has_VK_NV_device_generated_commands = 0;
	PFN_vkGetGeneratedCommandsMemoryRequirementsNV pfn_vkGetGeneratedCommandsMemoryRequirementsNV = 0;
	PFN_vkCmdPreprocessGeneratedCommandsNV pfn_vkCmdPreprocessGeneratedCommandsNV = 0;
	PFN_vkCmdExecuteGeneratedCommandsNV pfn_vkCmdExecuteGeneratedCommandsNV = 0;
	PFN_vkCmdBindPipelineShaderGroupNV pfn_vkCmdBindPipelineShaderGroupNV = 0;
	PFN_vkCreateIndirectCommandsLayoutNV pfn_vkCreateIndirectCommandsLayoutNV = 0;
	PFN_vkDestroyIndirectCommandsLayoutNV pfn_vkDestroyIndirectCommandsLayoutNV = 0;
#endif
}

#if defined(ERM_MAC)
#	pragma clang diagnostic pop
#endif
