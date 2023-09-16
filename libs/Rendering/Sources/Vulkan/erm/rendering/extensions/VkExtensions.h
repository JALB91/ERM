#pragma once

#include <vulkan/vulkan.h>

/* super load/reset */
void load_VK_EXTENSION_SUBSET(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr);
void reset_VK_EXTENSION_SUBSET();

/* loaders */
#if VK_EXT_debug_marker
#	define LOADER_VK_EXT_debug_marker 1
int load_VK_EXT_debug_marker(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr);
extern int has_VK_EXT_debug_marker;
#endif

#if VK_KHR_external_memory_win32 && defined(VK_USE_PLATFORM_WIN32_KHR)
#	define LOADER_VK_KHR_external_memory_win32 1
int load_VK_KHR_external_memory_win32(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr);
extern int has_VK_KHR_external_memory_win32;
#endif

#if VK_KHR_external_memory_fd
#	define LOADER_VK_KHR_external_memory_fd 1
int load_VK_KHR_external_memory_fd(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr);
extern int has_VK_KHR_external_memory_fd;
#endif

#if VK_KHR_external_semaphore_win32 && defined(VK_USE_PLATFORM_WIN32_KHR)
#	define LOADER_VK_KHR_external_semaphore_win32 1
int load_VK_KHR_external_semaphore_win32(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr);
extern int has_VK_KHR_external_semaphore_win32;
#endif

#if VK_KHR_external_semaphore_fd
#	define LOADER_VK_KHR_external_semaphore_fd 1
int load_VK_KHR_external_semaphore_fd(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr);
extern int has_VK_KHR_external_semaphore_fd;
#endif

#if VK_KHR_push_descriptor
#	define LOADER_VK_KHR_push_descriptor 1
int load_VK_KHR_push_descriptor(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr);
extern int has_VK_KHR_push_descriptor;
#endif

#if VK_KHR_create_renderpass2
#	define LOADER_VK_KHR_create_renderpass2 1
int load_VK_KHR_create_renderpass2(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr);
extern int has_VK_KHR_create_renderpass2;
#endif

#if VK_KHR_external_fence_win32 && defined(VK_USE_PLATFORM_WIN32_KHR)
#	define LOADER_VK_KHR_external_fence_win32 1
int load_VK_KHR_external_fence_win32(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr);
extern int has_VK_KHR_external_fence_win32;
#endif

#if VK_EXT_debug_utils
#	define LOADER_VK_EXT_debug_utils 1
int load_VK_EXT_debug_utils(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr);
extern int has_VK_EXT_debug_utils;
#endif

#if VK_EXT_sample_locations
#	define LOADER_VK_EXT_sample_locations 1
int load_VK_EXT_sample_locations(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr);
extern int has_VK_EXT_sample_locations;
#endif

#if VK_KHR_acceleration_structure
#	define LOADER_VK_KHR_acceleration_structure 1
int load_VK_KHR_acceleration_structure(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr);
extern int has_VK_KHR_acceleration_structure;
#endif

#if VK_KHR_ray_tracing_pipeline
#	define LOADER_VK_KHR_ray_tracing_pipeline 1
int load_VK_KHR_ray_tracing_pipeline(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr);
extern int has_VK_KHR_ray_tracing_pipeline;
#endif

#if VK_NV_shading_rate_image
#	define LOADER_VK_NV_shading_rate_image 1
int load_VK_NV_shading_rate_image(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr);
extern int has_VK_NV_shading_rate_image;
#endif

#if VK_NV_ray_tracing
#	define LOADER_VK_NV_ray_tracing 1
int load_VK_NV_ray_tracing(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr);
extern int has_VK_NV_ray_tracing;
#endif

#if VK_KHR_draw_indirect_count
#	define LOADER_VK_KHR_draw_indirect_count 1
int load_VK_KHR_draw_indirect_count(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr);
extern int has_VK_KHR_draw_indirect_count;
#endif

#if VK_EXT_calibrated_timestamps
#	define LOADER_VK_EXT_calibrated_timestamps 1
int load_VK_EXT_calibrated_timestamps(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr);
extern int has_VK_EXT_calibrated_timestamps;
#endif

#if VK_NV_mesh_shader
#	define LOADER_VK_NV_mesh_shader 1
int load_VK_NV_mesh_shader(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr);
extern int has_VK_NV_mesh_shader;
#endif

#if VK_NV_scissor_exclusive
#	define LOADER_VK_NV_scissor_exclusive 1
int load_VK_NV_scissor_exclusive(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr);
extern int has_VK_NV_scissor_exclusive;
#endif

#if VK_EXT_buffer_device_address
#	define LOADER_VK_EXT_buffer_device_address 1
int load_VK_EXT_buffer_device_address(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr);
extern int has_VK_EXT_buffer_device_address;
#endif

#if VK_NV_cooperative_matrix
#	define LOADER_VK_NV_cooperative_matrix 1
int load_VK_NV_cooperative_matrix(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr);
extern int has_VK_NV_cooperative_matrix;
#endif

#if VK_NV_coverage_reduction_mode
#	define LOADER_VK_NV_coverage_reduction_mode 1
int load_VK_NV_coverage_reduction_mode(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr);
extern int has_VK_NV_coverage_reduction_mode;
#endif

#if VK_KHR_buffer_device_address
#	define LOADER_VK_KHR_buffer_device_address 1
int load_VK_KHR_buffer_device_address(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr);
extern int has_VK_KHR_buffer_device_address;
#endif

#if VK_EXT_host_query_reset
#	define LOADER_VK_EXT_host_query_reset 1
int load_VK_EXT_host_query_reset(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr);
extern int has_VK_EXT_host_query_reset;
#endif

#if VK_KHR_pipeline_executable_properties
#	define LOADER_VK_KHR_pipeline_executable_properties 1
int load_VK_KHR_pipeline_executable_properties(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr);
extern int has_VK_KHR_pipeline_executable_properties;
#endif

#if VK_NV_device_generated_commands
#	define LOADER_VK_NV_device_generated_commands 1
int load_VK_NV_device_generated_commands(VkInstance instance, PFN_vkGetInstanceProcAddr getInstanceProcAddr, VkDevice device, PFN_vkGetDeviceProcAddr getDeviceProcAddr);
extern int has_VK_NV_device_generated_commands;
#endif
