cmake_minimum_required(VERSION 3.18)

project(VencordWorkspace)

include(FetchContent)

if(MINGW)
    FetchContent_Declare(
            Detours
            GIT_REPOSITORY https://github.com/0xeb/detours-cmake.git
            GIT_TAG master
    )

    FetchContent_MakeAvailable(Detours)
endif()

add_subdirectory(venject)
add_subdirectory(client)
