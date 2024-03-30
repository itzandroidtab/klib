# set minimum version of CMake.
cmake_minimum_required(VERSION 3.22)

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
    add_compile_options("-O3")
else()
    message("Release build")
    add_compile_options("-O3")
endif()

# change all floating point constants to single precision
add_compile_options("-fsingle-precision-constant")

# enable stack usage
add_compile_options(-fstack-usage)


# include the target cpu and set a target board
if (DEFINED TARGET_CPU)
    message("Building for target: ${TARGET_CPU}")

    # add the target directory
    add_subdirectory(${CMAKE_SOURCE_DIR}/targets/chip/${TARGET_CPU})

    # check if we have board with names for the pins
    if (DEFINED TARGET_BOARD)
        message("Building for target board: ${TARGET_BOARD}")
        target_compile_definitions(target_cpu PUBLIC "TARGET_BOARD=${TARGET_BOARD}")
    endif()

    # check if the user configured the fpu
    if (DEFINED TARGET_FPU_ENABLED)
        message("FPU is ${TARGET_FPU_ENABLED} for target: ${TARGET_CPU}")
        target_compile_definitions(target_cpu PUBLIC "TARGET_FPU_ENABLED=${TARGET_FPU_ENABLED}")
    else()
        message("FPU disabled for target: ${TARGET_CPU}")
        target_compile_definitions(target_cpu PUBLIC "TARGET_FPU_ENABLED=0")
    endif()

    # check if the user configured the low power sleep
    if (DEFINED TARGET_LOW_POWER_SLEEP)
        message("Low Power sleep is ${TARGET_LOW_POWER_SLEEP} for target: ${TARGET_CPU}")
        target_compile_definitions(target_cpu PUBLIC "TARGET_LOW_POWER_SLEEP=${TARGET_LOW_POWER_SLEEP}")
    else()
        message("Low Power sleep is disabled for target: ${TARGET_CPU}")
        target_compile_definitions(target_cpu PUBLIC "TARGET_LOW_POWER_SLEEP=0")
    endif()
else()
    message(FATAL_ERROR "No target cpu set. Use -DTARGET_CPU=\"cpu name\" to configure with a specific cpu" )
endif()

# add the klib library
add_subdirectory(${CMAKE_SOURCE_DIR}/klib)

# add the project sources
add_subdirectory(${CMAKE_SOURCE_DIR}/project)