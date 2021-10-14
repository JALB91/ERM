# FMOD_CORE
project(fmod_core)

add_library("${PROJECT_NAME}" SHARED IMPORTED GLOBAL)

set_target_properties(
    "${PROJECT_NAME}"
    PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/api/core/lib/libfmod.dylib"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/api/core/inc"
)

# FMOD_STUDIO
project(fmod_studio)

add_library("${PROJECT_NAME}" SHARED IMPORTED GLOBAL)

set_target_properties(
    "${PROJECT_NAME}"
    PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/api/studio/lib/libfmodstudio.dylib"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/api/studio/inc"
)