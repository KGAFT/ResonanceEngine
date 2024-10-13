cmake_minimum_required(VERSION 3.5)

project(ResonanceEngineNative)

set(VulkanSDKPath /opt/vulkan_sdk/1.3.296.0/x86_64/)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_CXX_STANDARD 23)

add_subdirectory(modules/VulkanLib)
add_subdirectory(modules/sdl)
add_subdirectory(modules/assimp)
add_subdirectory(modules/glm)
add_subdirectory(modules/glfw)

file(GLOB_RECURSE DEF_FILES src/main/cxx/*.h*)
file(GLOB_RECURSE SRC_FILES src/main/cxx/*.c*)

file(COPY src/main/glsl/ DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders/)
file(COPY assets DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
add_executable(ResonanceEngineNative-SDL ${DEF_FILES} ${SRC_FILES})
add_executable(ResonanceEngineNative-GLFW ${DEF_FILES} ${SRC_FILES})

target_include_directories(ResonanceEngineNative-SDL PUBLIC src/main/cxx NativeModules/include NativeModules/include_cfg)
target_include_directories(ResonanceEngineNative-GLFW PUBLIC src/main/cxx NativeModules/include NativeModules/include_cfg)

target_link_libraries(ResonanceEngineNative-SDL PUBLIC VulkanLib SDL3::SDL3-shared assimp glm::glm-header-only)
target_link_libraries(ResonanceEngineNative-GLFW PUBLIC VulkanLib glfw assimp glm::glm-header-only)

target_compile_definitions(ResonanceEngineNative-SDL PRIVATE SDLWindowUsed)
target_compile_definitions(ResonanceEngineNative-GLFW PRIVATE GLFWWindowUsed)
