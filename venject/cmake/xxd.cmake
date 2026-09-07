set(EMBEDDED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/embedded")
set(GENERATED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/gen/embedded")

# TODO: need powershell for msvc, or just keep using mingw..
add_custom_target(generate_embedded ALL
    COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/cmake/gen.sh"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    VERBATIM
)

set(EMBEDDED_GENERATE_TARGET generate_embedded PARENT_SCOPE)
