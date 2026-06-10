# enable the folder property for the whole project
set_property(GLOBAL PROPERTY USE_FOLDERS TRUE)

# provide the klib directory for the project
set(KLIB_DIR ${CMAKE_CURRENT_LIST_DIR})

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
    include(${KLIB_DIR}/targets/chip/${TARGET_CPU}/CMakeLists.txt)

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
    endif()

    if (DEFINED TARGET_BREAK_AT_RESERVED_HANDLER)
        message("Pausing on undefined handler call: ${TARGET_BREAK_AT_RESERVED_HANDLER} for target: ${TARGET_CPU}")
        target_compile_definitions(target_cpu PUBLIC "TARGET_BREAK_AT_RESERVED_HANDLER=${TARGET_BREAK_AT_RESERVED_HANDLER}")
    endif()
else()
    message(FATAL_ERROR "No target cpu set. Use -DTARGET_CPU=\"cpu name\" to configure with a specific cpu" )
endif()

# add the klib library
include(${KLIB_DIR}/klib/CMakeLists.txt)

# include the linkerscript generator needed to preprocess the linkerscripts
include(${KLIB_DIR}/targets/arm/linkerscript/linkerscript.cmake)