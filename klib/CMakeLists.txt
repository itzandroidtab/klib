# set the sources
set(SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/entry/entry.c
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

# link to the traget_cpu_options to get all the cpu flags
target_link_libraries(klib PUBLIC target_cpu_options)

# set the common flags
target_compile_options(klib PRIVATE "-Wall")
target_compile_options(klib PRIVATE "-Wextra")
target_compile_options(klib PRIVATE "-Werror")
target_compile_options(klib PRIVATE "-Wno-unused-function")

target_compile_options(klib PRIVATE "-Wno-maybe-uninitialized")
target_compile_options(klib PRIVATE "-Wno-unused-function")

target_compile_options(klib PUBLIC "-fomit-frame-pointer")
target_compile_options(klib PUBLIC "-fno-math-errno")

# set the c++ only options
target_compile_options(klib PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics>)
target_compile_options(klib PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>)
target_compile_options(klib PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-fconcepts>)
target_compile_options(klib PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-get-exception-ptr>)
target_compile_options(klib PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-atexit>)

# add other linker options
target_link_options(klib PUBLIC "-nostdlib")
target_link_options(klib PUBLIC "-nodefaultlibs")
target_link_options(klib PUBLIC "-nostartfiles")
target_link_options(klib PUBLIC "-Wl,--gc-sections")
target_link_options(klib PUBLIC "-Wl,-fatal-warnings")
target_link_options(klib PUBLIC "-Wl,-cref,-Map=klib.map")
target_link_options(klib PUBLIC "-Wl,--print-memory-usage")
target_link_options(klib PUBLIC "-fno-math-errno")

# Ignore warnings about rwx segments introduced in binutils 2.39
execute_process(COMMAND ${CMAKE_C_COMPILER} -print-prog-name=ld RESULT_VARIABLE RUN_C_RESULT OUTPUT_VARIABLE FULL_LD_PATH OUTPUT_STRIP_TRAILING_WHITESPACE)
if (${RUN_C_RESULT} EQUAL 0)
    execute_process(COMMAND ${FULL_LD_PATH} --help RESULT_VARIABLE RUN_LD_RESULT OUTPUT_VARIABLE LD_HELP_OUTPUT OUTPUT_STRIP_TRAILING_WHITESPACE)
    if (${RUN_LD_RESULT} EQUAL 0)
            set(RWX_WARNING "no-warn-rwx-segments")
            string(FIND "${LD_HELP_OUTPUT}" "${RWX_WARNING}" LD_RWX_WARNING_SUPPORTED)
            if (${LD_RWX_WARNING_SUPPORTED} GREATER -1)
                target_link_options(klib INTERFACE "LINKER:--${RWX_WARNING}")
            endif()
    endif()
endif()

# enabled segger support in the defines to prevent a 
# include of the segger files
if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/comm/segger/src/)
    target_compile_definitions(klib PUBLIC "KLIB_SEGGER_SUPPORT=1")
else()
    target_compile_definitions(klib PUBLIC "KLIB_SEGGER_SUPPORT=0")
endif()

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