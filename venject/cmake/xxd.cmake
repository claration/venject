find_program(BASH_EXECUTABLE bash REQUIRED)

add_custom_target(generate_embedded ALL
    COMMAND "${BASH_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake/gen.sh"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    VERBATIM
)
