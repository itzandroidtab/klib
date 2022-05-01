# export our linkerscript
set(TARGET_LINKERSCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/linkerscript.ld" PARENT_SCOPE)

# set the atsam3x8e cpu options as a seperate target so the driver layer can link agains klib
add_library (target_cpu_options INTERFACE)

# alias projectname::target_cpu_options to target_cpu_options
add_library(${PROJECT_NAME}::target_cpu_options ALIAS target_cpu_options)

# set the cpu options for the compiler
target_compile_options(target_cpu_options INTERFACE "-march=armv7-m")
target_compile_options(target_cpu_options INTERFACE "-mcpu=cortex-m3")
target_compile_options(target_cpu_options INTERFACE "-mthumb")

# set the cpu options for the linker
target_link_options(target_cpu_options INTERFACE "-march=armv7-m")
target_link_options(target_cpu_options INTERFACE "-mcpu=cortex-m3")
target_link_options(target_cpu_options INTERFACE "-mthumb")

# other compiler settings
target_compile_options(target_cpu_options INTERFACE "-Wno-attributes")
target_compile_options(target_cpu_options INTERFACE "-fno-non-call-exceptions")
target_compile_options(target_cpu_options INTERFACE "-fno-common")
target_compile_options(target_cpu_options INTERFACE "-ffunction-sections")
target_compile_options(target_cpu_options INTERFACE "-fdata-sections")
target_compile_options(target_cpu_options INTERFACE "-fno-exceptions")
target_compile_options(target_cpu_options INTERFACE "-fno-asynchronous-unwind-tables")


# cpu atsam3x8e target drivers 
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../arm/vector_table/cortex-m3.cpp
)

set(HEADERS_PRIVATE 

)

set(HEADERS_PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/rstc.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/watchdog.hpp
)

# add the target_cpu library
add_library (target_cpu OBJECT 
    ${SOURCES}
    ${HEADERS_PUBLIC}
    ${HEADERS_PRIVATE}
)

# alias projectname::target to klib
add_library(${PROJECT_NAME}::target_cpu ALIAS target_cpu)

# enable C++20 support for the library
target_compile_features(target_cpu PUBLIC cxx_std_20)

# set the target_cpu for klib
target_compile_definitions(target_cpu PUBLIC "TARGET_CPU=atsam3x8e")

# include for the generated file from svd and for the coprocessors
target_include_directories(target_cpu PUBLIC "${CMAKE_SOURCE_DIR}/targets/arm/")

# add target specific compile options
target_compile_options(target_cpu PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-Wno-volatile>)

# link to klib and target_cpu_options
target_link_libraries(target_cpu PUBLIC target_cpu_options)
target_link_libraries(target_cpu PUBLIC klib)

# Add definitions for targets
# Values:
#   - Debug  : -DTARGET_DEBUG=1
#   - Release: -DTARGET_DEBUG=0
#   - others : -DTARGET_DEBUG=0
target_compile_definitions(target_cpu PUBLIC
"${PROJECT_NAME_UPPERCASE}_DEBUG=$<CONFIG:Debug>")

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