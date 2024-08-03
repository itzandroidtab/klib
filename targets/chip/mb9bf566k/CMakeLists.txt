# export our linkerscript
set(TARGET_LINKERSCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/linkerscript.ld" PARENT_SCOPE)

# set the atsam4s2b cpu options as a seperate target so the driver layer can link agains klib
add_library(target_cpu_options INTERFACE)
set_target_properties(target_cpu_options PROPERTIES FOLDER "klib")

# alias projectname::target_cpu_options to target_cpu_options
add_library(${PROJECT_NAME}::target_cpu_options ALIAS target_cpu_options)

# include the arm directory for all the cmsis files
target_include_directories(target_cpu_options INTERFACE ${CMAKE_SOURCE_DIR}/targets/arm/)

# set the cpu options for the compiler
target_compile_options(target_cpu_options INTERFACE "-march=armv7e-m")
target_compile_options(target_cpu_options INTERFACE "-mcpu=cortex-m4")
target_compile_options(target_cpu_options INTERFACE "-mthumb")
if (DEFINED TARGET_FPU_ENABLED)
    target_compile_options(target_cpu_options INTERFACE "-mfloat-abi=hard")
    target_compile_options(target_cpu_options INTERFACE "-mfpu=fpv4-sp-d16")
endif()

# set the cpu options for the linker
target_link_options(target_cpu_options INTERFACE "-march=armv7e-m")
target_link_options(target_cpu_options INTERFACE "-mcpu=cortex-m4")
target_link_options(target_cpu_options INTERFACE "-mthumb")
if (DEFINED TARGET_FPU_ENABLED)
    target_link_options(target_cpu_options INTERFACE "-mfloat-abi=hard")
    target_link_options(target_cpu_options INTERFACE "-mfpu=fpv4-sp-d16")
endif()

# other compiler settings
target_compile_options(target_cpu_options INTERFACE "-Wno-attributes")
target_compile_options(target_cpu_options INTERFACE "-fno-non-call-exceptions")
target_compile_options(target_cpu_options INTERFACE "-fno-common")
target_compile_options(target_cpu_options INTERFACE "-ffunction-sections")
target_compile_options(target_cpu_options INTERFACE "-fdata-sections")
target_compile_options(target_cpu_options INTERFACE "-fno-exceptions")
target_compile_options(target_cpu_options INTERFACE "-fno-asynchronous-unwind-tables")


# cpu atsam4s2b target drivers 
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../../arm/vector_table/cortex-m4.cpp
)

set(HEADERS_PRIVATE 

)

set(HEADERS_PUBLIC 

)

# add the target_cpu library
add_library(target_cpu OBJECT 
    ${SOURCES}
    ${HEADERS_PUBLIC}
    ${HEADERS_PRIVATE}
)
set_target_properties(target_cpu PROPERTIES FOLDER "klib")

# alias projectname::target_cpu to target_cpu
add_library(${PROJECT_NAME}::target_cpu ALIAS target_cpu)

# enable C++20 support for the library
target_compile_features(target_cpu PUBLIC cxx_std_20)

# set the target_cpu for klib
get_filename_component(TARGET_CPU_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(TARGET_CPU ${TARGET_CPU_FOLDER} PARENT_SCOPE)
target_compile_definitions(target_cpu PUBLIC "TARGET_CPU=${TARGET_CPU}")

# add target specific compile options
target_compile_options(target_cpu PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-Wno-volatile>)

# link to klib and target_cpu_options
target_link_libraries(target_cpu PUBLIC target_cpu_options)
target_link_libraries(target_cpu PUBLIC klib)

# Global includes. Used by all targets
# Note:
#   - header can be included by C++ code `#include <target/target.hpp>`
#   - header location in project: ${CMAKE_CURRENT_BINARY_DIR}/generated_headers
target_include_directories(
    target_cpu PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${GENERATED_HEADERS_DIR}>"
)