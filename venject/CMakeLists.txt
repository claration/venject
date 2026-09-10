cmake_minimum_required(VERSION 3.18)

project(vencord_hook LANGUAGES C CXX ASM)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# =========================
# MARK: CMAKE DEPENDENCIES
# =========================

if(MINGW)
    FetchContent_Declare(
        Detours
        GIT_REPOSITORY https://github.com/0xeb/detours-cmake.git
        GIT_TAG master
    )

    FetchContent_MakeAvailable(Detours)
else()
    find_program(PATCH patch REQUIRED)
    set(DOBBY_DEBUG OFF CACHE BOOL "" FORCE)
    set(DOBBY_GENERATE_SHARED OFF CACHE BOOL "" FORCE)
    set(DOBBY_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
    set(DOBBY_BUILD_TEST OFF CACHE BOOL "" FORCE)
    set(NearBranch ON CACHE BOOL "" FORCE)
    set(FullFloatingPointRegisterPack OFF CACHE BOOL "" FORCE)
    set(Plugin.SymbolResolver ON CACHE BOOL "" FORCE)
    set(Plugin.ImportTableReplace OFF CACHE BOOL "" FORCE)
    set(Plugin.Android.BionicLinkerUtil OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        Dobby
        GIT_REPOSITORY https://github.com/jmpews/Dobby.git
        GIT_TAG master
        PATCH_COMMAND
            ${CMAKE_COMMAND} -E chdir <SOURCE_DIR>
            ${PATCH} --forward --ignore-whitespace -p1
            -i "${CMAKE_CURRENT_SOURCE_DIR}/patches/dobby/0001-fix-compilations.patch" || true
    )

    FetchContent_MakeAvailable(Dobby)
endif()

# =========================
# MARK: TARGET
# =========================

message(STATUS "Using XXD on this shit.")
include(cmake/Xxd.cmake)

file(GLOB_RECURSE VENCORD_HOOK_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

add_library(${PROJECT_NAME} SHARED
    ${VENCORD_HOOK_SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
)

if(MINGW)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        lib_detours
    )
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE
        dobby
        dl
    )
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Os
        -ffunction-sections
        -fdata-sections
    )

    target_link_options(${PROJECT_NAME} PRIVATE
        -Wl,--gc-sections
    )
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

install(
    TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION bin
    RUNTIME DESTINATION bin
)
