cmake_minimum_required(VERSION 3.18)

project(VencordWorkspace)

include(FetchContent)

if(MINGW)
	add_link_options("-Wl,--allow-multiple-definition")

	FetchContent_Declare(
		Detours
		GIT_REPOSITORY https://github.com/0xeb/detours-cmake.git
		GIT_TAG d558db8c1ff3203302b6ba15517a678e127243a8
	)

	FetchContent_MakeAvailable(Detours)
endif()

add_subdirectory(venject)
add_subdirectory(client)
