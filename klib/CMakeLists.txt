# set the sources
set(SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/entry/entry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/entry/polyfill.c
)

set(HEADERS_PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/comm/streams/stream_base.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/comm/streams/combined_stream.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/comm/streams/istream.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/comm/streams/ostream.hpp
)

set(HEADERS_PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/irq.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/klib.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/stream.hpp
)

# check if we need to add the segger rtt library
if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/comm/segger/src/)
    message("Segger RTT support enabled")

    # append the header implementation to the public headers
    set(HEADERS_PUBLIC ${HEADERS_PUBLIC} ${CMAKE_CURRENT_SOURCE_DIR}/comm/segger/rtt.hpp)
    set(HEADERS_PRIVATE ${HEADERS_PUBLIC} ${CMAKE_CURRENT_SOURCE_DIR}/comm/streams/rtt_stream.hpp)

    # add the segger library source files
    set(SOURCES ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/comm/segger/src/RTT/SEGGER_RTT.c)
else()
    message("Segger RTT support disabled")
endif()

# add the library
add_library (klib OBJECT
    ${SOURCES}
    ${HEADERS_PUBLIC}
    ${HEADERS_PRIVATE}
)

# alias projectname::klib to klib
add_library(${PROJECT_NAME}::klib ALIAS klib)

# enable C++20 support for the library
target_compile_features(klib PUBLIC cxx_std_20)

# link to the cpu target to get all the compile options and linker options 
# in the library
target_link_libraries(klib PUBLIC target_cpu)

# set the common flags
target_compile_options(klib PRIVATE "-Wall")
target_compile_options(klib PRIVATE "-Werror")

target_compile_options(klib PRIVATE "-Wno-maybe-uninitialized")
target_compile_options(klib PRIVATE "-Wno-unused-local-typedefs")
target_compile_options(klib PRIVATE "-Wno-unused-but-set-variable")
target_compile_options(klib PRIVATE "-Wno-unused-function")
target_compile_options(klib PRIVATE "-fomit-frame-pointer")

target_compile_options(klib PUBLIC "-nostdlib")

# set the c++ only options
target_compile_options(klib PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics>)
target_compile_options(klib PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>)
target_compile_options(klib PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-fconcepts>)
target_compile_options(klib PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-get-exception-ptr>)

# add other linker options
target_link_options(klib PUBLIC "-nostdlib")
target_link_options(klib PUBLIC "-nodefaultlibs")
target_link_options(klib PUBLIC "-nostartfiles")
target_link_options(klib PUBLIC "-Wl,--gc-sections")
target_link_options(klib PUBLIC "-Wl,-fatal-warnings")
target_link_options(klib PUBLIC "-Wl,-cref,-Map=klib.map")

# Add definitions for targets
# Values:
#   - Debug  : -DKLIB_DEBUG=1
#   - Release: -DKLIB_DEBUG=0
#   - others : -DKLIB_DEBUG=0
target_compile_definitions(klib PUBLIC
"${PROJECT_NAME_UPPERCASE}_DEBUG=$<CONFIG:Debug>")

# Global includes. Used by all targets
# Note:
#   - header can be included by C++ code `#include <klib/klib.hpp>`
#   - header location in project: ${CMAKE_CURRENT_BINARY_DIR}/generated_headers
target_include_directories(
    klib PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${GENERATED_HEADERS_DIR}>"
)