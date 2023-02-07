cmake_minimum_required(VERSION 3.12.0 FATAL_ERROR)
project(spectre_api C CXX)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake)

include(CompileSodium)

set(sodium_USE_STATIC_LIBS TRUE)
find_package(sodium REQUIRED)

add_library(spectre_api STATIC
	aes.c
	spectre-algorithm.c
	spectre-algorithm_v0.c
	spectre-algorithm_v1.c
	spectre-algorithm_v2.c
	spectre-algorithm_v3.c
	spectre-marshal-util.c
	spectre-marshal.c
	spectre-types.c
	spectre-util.c
)

set_property(TARGET spectre_api PROPERTY C_STANDARD 17)
set_property(TARGET spectre_api PROPERTY C_STANDARD_REQUIRED ON)
target_include_directories(spectre_api INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(spectre_api PRIVATE SPECTRE_SODIUM)
target_link_libraries(spectre_api PRIVATE sodium)
