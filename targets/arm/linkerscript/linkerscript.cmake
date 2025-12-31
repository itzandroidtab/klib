# helper function to preprocess a linker script. This allows 
# using C preprocessor in the linkerscript files (including 
# macroes, conditionals, etc)
function(add_linkerscript target linkerscript options)
    set(output "${CMAKE_BINARY_DIR}/linkerscript.ld")
    set(CURRENT_DIR "${CMAKE_SOURCE_DIR}/targets/arm/linkerscript")

    # run the preprocessor on the linkerscript
    add_custom_command(
        OUTPUT ${output}
        # run the linkerscript through the C preprocessor. Force include the base linkerscript template.
        COMMAND ${CMAKE_C_COMPILER} -E -P -x c ${linkerscript} -o ${output} -include ${CURRENT_DIR}/linkerscript.ld ${options}
        DEPENDS ${linkerscript} ${CURRENT_DIR}/linkerscript.ld
        COMMENT "Preprocessing linker script ${linkerscript}"
        VERBATIM
    )

    # create a custom target to generate the linkerscript
    add_custom_target(generate_linkerscript DEPENDS "${output}")
    add_dependencies(${target} generate_linkerscript)

    # add the linkerscript to the target
    target_link_options(${target} PUBLIC "-T${output}")
    set_target_properties(${target} PROPERTIES LINK_DEPENDS ${output})
endfunction()

