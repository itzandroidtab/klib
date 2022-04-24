# set minimum version of CMake.
cmake_minimum_required(VERSION 3.13)

# The Generic system name is used for embedded targets (targets without OS) in
# CMake
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# Supress Error when trying to test the compiler
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(BUILD_SHARED_LIBS OFF)

# set project name and version
project(klib VERSION 0.0.1)

# enable assembly
enable_language(ASM)

# check if we need to enable the debug features
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Debug symbols enabled")
    add_compile_options("-g")
    add_compile_options("-Og")
elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    message("Minimal release build")
    add_compile_options("-Os")
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    message("Release build with debugging symbols")
    add_compile_options("-g")
    add_compile_options("-Os")
else()
    message("Release build ${CMAKE_BUILD_TYPE}")
    add_compile_options("-O2")
endif()

add_compile_options("-fsingle-precision-constant")

# enable stack usage
add_compile_options(-fstack-usage)

# include the target cpu
add_subdirectory(${CMAKE_SOURCE_DIR}/targets/atsam3x8e)

# add the klib library
add_subdirectory(${CMAKE_SOURCE_DIR}/klib)

# add the project sources
add_subdirectory(${CMAKE_SOURCE_DIR}/project)