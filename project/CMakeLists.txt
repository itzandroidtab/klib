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

# set the interrupt implementation
target_compile_definitions(klib PUBLIC "KLIB_IRQ=irq_ram")
# target_compile_definitions(klib PUBLIC "KLIB_IRQ=irq_flash")

# set the default cout/cin
target_compile_definitions(klib PUBLIC "KLIB_DEFAULT_COUT=rtt")
target_compile_definitions(klib PUBLIC "KLIB_DEFAULT_CIN=rtt")

# override the rtt buffer size
target_compile_definitions(klib PUBLIC "BUFFER_SIZE_UP=128")
target_compile_definitions(klib PUBLIC "BUFFER_SIZE_DOWN=16")

# link the kproject to klib and the cpu target
target_link_libraries(kproject PUBLIC klib)
target_link_libraries(kproject PUBLIC target_cpu)

# Libraries to link for all targets
target_link_libraries(kproject PUBLIC m)

# link to the linkerscript of the target cpu
target_link_options(kproject PUBLIC "-T${TARGET_LINKERSCRIPT}")
set_target_properties(kproject PROPERTIES LINK_DEPENDS ${TARGET_LINKERSCRIPT})

# add the project directory to the include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}) 

# Custom commands for processing the build binary and show some statistics and debug info 
add_custom_command(TARGET kproject DEPENDS ${CMAKE_BINARY_DIR}/klib.elf POST_BUILD COMMAND arm-none-eabi-objcopy ARGS -O binary -R .bss -R .stack klib.elf klib.bin)
add_custom_command(TARGET kproject DEPENDS ${CMAKE_BINARY_DIR}/klib.elf POST_BUILD COMMAND arm-none-eabi-objdump ARGS -C -S klib.elf > klib.lss)
add_custom_command(TARGET kproject DEPENDS ${CMAKE_BINARY_DIR}/klib.elf POST_BUILD COMMAND arm-none-eabi-objdump ARGS -C -sj .bss -sj .data -sj .rodata -sj .vectors -S klib.elf > klib.memory)
add_custom_command(TARGET kproject DEPENDS ${CMAKE_BINARY_DIR}/klib.elf POST_BUILD COMMAND arm-none-eabi-size ARGS -A klib.elf -x)
