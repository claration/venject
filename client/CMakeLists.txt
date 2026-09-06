cmake_minimum_required(VERSION 3.18)

project(samaraqt VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 CONFIG REQUIRED COMPONENTS
	Core
	Gui
	Network
	Qml
	Quick
	QuickControls2
)

# =========================
# MARK: TARGET
# =========================

qt_standard_project_setup()

qt_add_executable(${PROJECT_NAME}
    src/main.cpp
    src/launch.cpp
    src/dl.cpp
    src/paths/linux.cpp
)

qt_add_qml_module(${PROJECT_NAME}
    URI ${PROJECT_NAME}
    VERSION 1.0

    QML_FILES 
		qml/Main.qml

    RESOURCES
        resources/canary.webp
        resources/stable.webp
)

target_include_directories(${PROJECT_NAME} PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	Qt6::Core
	Qt6::Gui
	Qt6::Network
	Qt6::Qml
	Qt6::Quick
	Qt6::QuickControls2
)

set_target_properties(${PROJECT_NAME} PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

if(LINUX)
    target_link_options(${PROJECT_NAME} PRIVATE
		-static-libgcc
		-static-libstdc++
    )
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(LINUX)
        target_link_options(${PROJECT_NAME} PRIVATE
            -Wl,--gc-sections
            -Wl,--strip-all
        )
    endif()
endif()

# =========================
# MARK: CPACK
# =========================

include(cmake/ConfigureCPack.cmake)
