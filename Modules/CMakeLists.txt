# Cmake version
cmake_minimum_required(VERSION 3.15...3.17)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/Utils")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/Audio")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/Math")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/Engine")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/Main")
