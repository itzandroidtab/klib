# export our linkerscript
set(TARGET_LINKERSCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/linkerscript.ld" PARENT_SCOPE)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../arm/vector_table/cortex-m3.cpp
)

set(HEADERS_PRIVATE 
    # This file needs to be generated using the cmsis svd tool
    ${CMAKE_CURRENT_SOURCE_DIR}/ATSAM3X8E.h
)

set(HEADERS_PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/rstc.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/watchdog.hpp
)

# add the library
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

# set the cpu options for the compiler
target_compile_definitions(target_cpu PUBLIC "__SAM3X8E__")
target_compile_options(target_cpu PUBLIC "-march=armv7-m")
target_compile_options(target_cpu PUBLIC "-mcpu=cortex-m3")
target_compile_options(target_cpu PUBLIC "-mthumb")

# other compile options
target_compile_options(target_cpu PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-Wno-volatile>)

# set the cpu options for the linker
target_link_options(target_cpu PUBLIC "-march=armv7-m")
target_link_options(target_cpu PUBLIC "-mcpu=cortex-m3")
target_link_options(target_cpu PUBLIC "-mthumb")

# other compiler settings
target_compile_options(target_cpu PUBLIC "-Wno-attributes")
target_compile_options(target_cpu PUBLIC "-fno-non-call-exceptions")
target_compile_options(target_cpu PUBLIC "-fno-common")
target_compile_options(target_cpu PUBLIC "-ffunction-sections")
target_compile_options(target_cpu PUBLIC "-fdata-sections")
target_compile_options(target_cpu PUBLIC "-fno-exceptions")
target_compile_options(target_cpu PUBLIC "-fno-asynchronous-unwind-tables")

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