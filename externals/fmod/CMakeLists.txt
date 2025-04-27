if(ERM_HOST_WINDOWS)
    set(FMOD_CORE_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/api/core/lib/Win/x64/fmod_vc.lib")
    set(FMOD_CORE_LIBRARY_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/api/core/lib/Win/x64/")
    set(FMOD_STUDIO_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/api/studio/lib/Win/x64/fmodstudio_vc.lib")
    set(FMOD_STUDIO_LIBRARY_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/api/studio/lib/Win/x64/")
elseif(ERM_HOST_OSX)
    set(FMOD_CORE_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/api/core/lib/OSX/libfmod.dylib")
    set(FMOD_CORE_LIBRARY_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/api/core/lib/OSX/libfmod.dylib")
    set(FMOD_STUDIO_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/api/studio/lib/OSX/libfmodstudio.dylib")
    set(FMOD_STUDIO_LIBRARY_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/api/studio/lib/OSX/libfmodstudio.dylib")
else()
    message(FATAL_ERROR "Unsupported Platform ${ERM_HOST_PLATFORM}")
endif()

# FMOD_CORE
project(fmod_core)

add_library("${PROJECT_NAME}" SHARED IMPORTED GLOBAL)

set_target_properties(
    "${PROJECT_NAME}"
    PROPERTIES
        IMPORTED_IMPLIB "${FMOD_CORE_IMPLIB}"
        IMPORTED_LOCATION "${FMOD_CORE_LIBRARY_LOCATION}"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/api/core/inc"
)

# FMOD_STUDIO
project(fmod_studio)

add_library("${PROJECT_NAME}" SHARED IMPORTED GLOBAL)

set_target_properties(
    "${PROJECT_NAME}"
    PROPERTIES
        IMPORTED_IMPLIB "${FMOD_STUDIO_IMPLIB}"
        IMPORTED_LOCATION "${FMOD_STUDIO_LIBRARY_LOCATION}"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/api/studio/inc"
)
