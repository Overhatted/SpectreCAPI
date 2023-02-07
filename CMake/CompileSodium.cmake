set(COMPILED_VERSION_INTERNAL 1)

include(FetchContent)

FetchContent_Declare(
	Sodium
	GIT_REPOSITORY https://github.com/jedisct1/libsodium.git
	GIT_TAG d69a2342bccb98a3c28c0b7d5e4e6f3b8c789621#Branch: stable
)

FetchContent_GetProperties(Sodium)
if(NOT sodium_POPULATED)
	FetchContent_Populate(Sodium)
endif()

if(NOT "${Sodium_COMPILED_VERSION}" STREQUAL ${COMPILED_VERSION_INTERNAL})
	if(WIN32)
		file(WRITE ${sodium_BINARY_DIR}/CMakeLists.txt "\
execute_process(COMMAND \${CMAKE_VS_MSBUILD_COMMAND} \"${sodium_SOURCE_DIR}/builds/msvc/vs2022/libsodium.sln\" -property:PlatformToolset=\${CMAKE_VS_PLATFORM_TOOLSET} -property:WindowsTargetPlatformVersion=\${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION} -property:Configuration=StaticDebug -property:Platform=x64)
execute_process(COMMAND \${CMAKE_VS_MSBUILD_COMMAND} \"${sodium_SOURCE_DIR}/builds/msvc/vs2022/libsodium.sln\" -property:PlatformToolset=\${CMAKE_VS_PLATFORM_TOOLSET} -property:WindowsTargetPlatformVersion=\${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION} -property:Configuration=StaticRelease -property:Platform=x64)
")

		execute_process(COMMAND ${CMAKE_COMMAND} . WORKING_DIRECTORY ${sodium_BINARY_DIR})

		file(COPY ${sodium_SOURCE_DIR}/src/libsodium/include DESTINATION ${sodium_BINARY_DIR}/install)
		file(COPY ${sodium_SOURCE_DIR}/bin/x64 DESTINATION ${sodium_BINARY_DIR}/install/lib)
	endif()

	set(Sodium_COMPILED_VERSION ${COMPILED_VERSION_INTERNAL} CACHE INTERNAL "")
endif()

set(sodium_ROOT ${sodium_BINARY_DIR}/install)
