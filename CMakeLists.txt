cmake_minimum_required(VERSION 3.5)

project(ResonanceEngineNative)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_CXX_STANDARD 23)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

add_subdirectory(modules/VulkanLib)
add_subdirectory(modules/sdl)
add_subdirectory(modules/assimp)
add_subdirectory(modules/glm)

file(GLOB_RECURSE DEF_FILES src/main/cxx/*.h*)
file(GLOB_RECURSE SRC_FILES src/main/cxx/*.c*)

file(COPY src/main/glsl/ DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders/)

add_executable(ResonanceEngineNative ${DEF_FILES} ${SRC_FILES})

target_include_directories(ResonanceEngineNative PUBLIC src/main/cxx NativeModules/include NativeModules/include_cfg)

target_link_libraries(ResonanceEngineNative PUBLIC VulkanLib SDL3::SDL3-shared assimp glm)

