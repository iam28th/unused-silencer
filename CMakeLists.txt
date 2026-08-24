cmake_minimum_required(VERSION 4.3.3)
project(silencer)

set(SILENCER_LLVM_TOOLS_DIR "/usr/bin" CACHE PATH "Where llvm project binaries (clang, FileCheck, lit etc) are located. When using built-from-source llvm, this should point to llvm-project/{build-dir}/bin")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")  # where packages are searched first
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_LIBRARY_OUTPUT_DIRECTORY: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
message(STATUS "CMAKE_RUNTIME_OUTPUT_DIRECTORY: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

find_package(Clang REQUIRED CONFIG)
message(STATUS "Clang: ${CLANG}")
message(STATUS "Clang include dirs: ${CLANG_INCLUDE_DIRS}")

# LLVM/Clang is normally built without RTTI. Be consistent with that (c)
# - consistency is good n'all, but also modules will fail to load otherwise ...
message(STATUS "LLVM_ENABLE_RTTI: ${LLVM_ENABLE_RTTI}")
if(NOT LLVM_ENABLE_RTTI)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
	message(STATUS "rtti disabled")
else ()
	message(STATUS "rtti enabled")
endif()

set(SILENCER_PLUGIN_SONAME Silencer)
add_subdirectory(lib)
add_subdirectory(silencer_tool)
add_subdirectory(tests)
