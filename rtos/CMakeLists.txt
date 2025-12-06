# set the sources
set(SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/syscall.cpp
)

set(HEADERS_PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/base_task.hpp
)

set(HEADERS_PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/scheduler.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/syscall.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/task.hpp
)

# add the library
add_library(rtos OBJECT
    ${SOURCES}
    ${HEADERS_PUBLIC}
    ${HEADERS_PRIVATE}
)
set_target_properties(rtos PROPERTIES FOLDER "rtos")

# alias projectname::rtos to rtos
add_library(${PROJECT_NAME}::rtos ALIAS rtos)

# enable C++20 support for the library
target_compile_features(rtos PUBLIC cxx_std_23)

# link to the traget_cpu_options to get all the cpu flags
target_link_libraries(rtos PUBLIC target_cpu_options target_cpu klib)

# Global includes. Used by all targets
# Note:
#   - header can be included by C++ code `#include <klib/klib.hpp>`
#   - header location in project: ${CMAKE_CURRENT_BINARY_DIR}/generated_headers
target_include_directories(
    rtos PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${GENERATED_HEADERS_DIR}>"
)
