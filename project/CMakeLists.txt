# The Generic system name is used for embedded targets (targets without OS) in
# CMake
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# Supress Error when trying to test the compiler
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(BUILD_SHARED_LIBS OFF)

# set minimum version of CMake.
cmake_minimum_required(VERSION 3.22)

# set project name and version
project(klib VERSION 0.0.1)

# include the klib library. Note: This needs to point to your klib installation folder
include(../klib/CMakeLists.txt)

# set the sources
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp

    # example for including the startup.cpp
    # ${CMAKE_SOURCE_DIR}/targets/chip/${TARGET_CPU}/startup.cpp
)

set(HEADERS)

# add our executable
add_executable(klib_project
    ${SOURCES} ${HEADERS}
)

# set the output filename
set_target_properties(klib_project PROPERTIES OUTPUT_NAME "klib" SUFFIX ".elf")

# set the interrupt implementation
target_compile_definitions(klib PUBLIC "KLIB_IRQ=irq_ram")
# target_compile_definitions(klib PUBLIC "KLIB_IRQ=irq_hooked")
# target_compile_definitions(klib PUBLIC "KLIB_IRQ=irq_flash")

# set the default cout/cin
target_compile_definitions(klib PUBLIC "KLIB_DEFAULT_COUT=rtt")
target_compile_definitions(klib PUBLIC "KLIB_DEFAULT_CIN=rtt")

# override the rtt buffer size (not required and not used when no segger rtt is used)
target_compile_definitions(klib PUBLIC "BUFFER_SIZE_UP=256")
target_compile_definitions(klib PUBLIC "BUFFER_SIZE_DOWN=16")

# set if we should support certain callbacks
target_compile_definitions(klib PUBLIC "SYSTICK_CALLBACK_ENABLED=false")

# link the klib_project to klib and the cpu target
target_link_libraries(klib_project PUBLIC klib)
target_link_libraries(klib_project PUBLIC target_cpu)

# Libraries to link for all targets
target_link_libraries(klib_project PUBLIC m)

# get the linkerscript we should link against
get_property(TARGET_LINKERSCRIPT GLOBAL PROPERTY TARGET_LINKERSCRIPT)

# link to the linkerscript of the target cpu
target_link_options(klib_project PUBLIC "-T${TARGET_LINKERSCRIPT}")
set_target_properties(klib_project PROPERTIES LINK_DEPENDS ${TARGET_LINKERSCRIPT})

# add the project directory to the include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Custom commands for processing the build binary and show some statistics and debug info
add_custom_command(TARGET klib_project DEPENDS ${CMAKE_BINARY_DIR}/klib.elf POST_BUILD COMMAND arm-none-eabi-objcopy ARGS -O binary -R .bss -R .stack klib.elf klib.bin)
add_custom_command(TARGET klib_project DEPENDS ${CMAKE_BINARY_DIR}/klib.elf POST_BUILD COMMAND arm-none-eabi-objcopy ARGS -O ihex -R .bss -R .stack klib.elf klib.hex)
add_custom_command(TARGET klib_project DEPENDS ${CMAKE_BINARY_DIR}/klib.elf POST_BUILD COMMAND arm-none-eabi-objdump ARGS -C -S klib.elf > klib.lss)
add_custom_command(TARGET klib_project DEPENDS ${CMAKE_BINARY_DIR}/klib.elf POST_BUILD COMMAND arm-none-eabi-objdump ARGS -C -sj .bss -sj .data -sj .rodata -sj .vectors -S klib.elf > klib.memory)
add_custom_command(TARGET klib_project DEPENDS ${CMAKE_BINARY_DIR}/klib.elf POST_BUILD COMMAND arm-none-eabi-size ARGS -A klib.elf -x)
