# set the sources
set(SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

set(HEADERS)

# add our executable
add_executable(kproject
    ${SOURCES} ${HEADERS}
)

# set the output filename
set_target_properties(kproject PROPERTIES OUTPUT_NAME "klib" SUFFIX ".elf")

# link the kproject to klib and the cpu target
target_link_libraries(kproject PUBLIC klib)
target_link_libraries(kproject PUBLIC target_cpu)

# Libraries to link for all targets
target_link_libraries(kproject PUBLIC m)

# link to the linkerscript of the target cpu
target_link_options(kproject PUBLIC "-T${TARGET_LINKERSCRIPT}")

# add the project directory to the include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}) 

# Custom commands for processing the build binary and show some statistics and debug info 
add_custom_command(TARGET kproject DEPENDS ${CMAKE_BINARY_DIR}/klib.elf POST_BUILD COMMAND arm-none-eabi-objcopy ARGS -O binary -R .bss -R .stack klib.elf klib.bin)
add_custom_command(TARGET kproject DEPENDS ${CMAKE_BINARY_DIR}/klib.elf POST_BUILD COMMAND arm-none-eabi-objdump ARGS -S klib.elf > klib.lss)
add_custom_command(TARGET kproject DEPENDS ${CMAKE_BINARY_DIR}/klib.elf POST_BUILD COMMAND arm-none-eabi-size ARGS -A klib.elf -x)