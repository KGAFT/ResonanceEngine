cmake_minimum_required(VERSION 3.5)

project(ResonanceEngineNative)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(JNI REQUIRED)
file(GLOB_RECURSE LIN_SHARED_LIBS NativeModules/bin/*.so )
file(GLOB_RECURSE WIN_SHARED_LIBS NativeModules/bin/*.dll )
file(GLOB_RECURSE WIN_STATIC_LIBS NativeModules/lib/*.lib )

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/JavaModules)

add_subdirectory(modules/VulkanLib)
add_subdirectory(modules/sdl)

file(GLOB_RECURSE DEF_FILES src/main/cxx/*.h*)
file(GLOB_RECURSE SRC_FILES src/main/cxx/*.c*)
file(GLOB_RECURSE JAVA_INTERFACES src/main/java/com/kgaft/ResonanceEngine/Native/*.java)

execute_process(COMMAND javac -h ${CMAKE_CURRENT_SOURCE_DIR}/JniInterfaces ${JAVA_INTERFACES})

add_library(ResonanceEngineNative SHARED ${DEF_FILES} ${SRC_FILES})

target_include_directories(ResonanceEngineNative PUBLIC src/main/cxx ${JNI_INCLUDE_DIRS} JniInterfaces NativeModules/include NativeModules/include_cfg)

target_link_libraries(ResonanceEngineNative PUBLIC VulkanLib SDL3::SDL3-shared ${LIN_SHARED_LIBS} ${WIN_SHARED_LIBS} ${WIN_STATIC_LIBS})