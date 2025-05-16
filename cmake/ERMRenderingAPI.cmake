if(NOT ERM_RENDERING_API)
    set(ERM_RENDERING_API "VULKAN" CACHE STRING "" FORCE)
endif()

if("${ERM_RENDERING_API}" STREQUAL "VULKAN")
    include(cmake/rendering_apis/ERMRenderingAPIVulkan.cmake)
elseif("${ERM_RENDERING_API}" STREQUAL "DX12")
    include(cmake/rendering_apis/ERMRenderingAPIDX12.cmake)
elseif("${ERM_RENDERING_API}" STREQUAL "OPEN_GL")
    include(cmake/rendering_apis/ERMRenderingAPIOpenGL.cmake)
else()
    message(FATAL_ERROR "Rendering API not supported ${ERM_RENDERING_API}")
endif()

if(ERM_DX12 AND NOT ERM_HOST_WINDOWS)
    message(FATAL_ERROR "DX12 rendering API is only available on windows")
endif()

message(STATUS "Using ${ERM_RENDERING_API} rendering API")
