cmake_minimum_required(VERSION 3.18)

project(VencordLauncher VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(VERSION "1.0.0" CACHE STRING "Version")

find_package(Qt6 CONFIG REQUIRED COMPONENTS
	Core
	Gui
	Network
	Qml
	Quick
	QuickControls2
	LinguistTools
	Widgets
)

# =========================
# MARK: TARGET
# =========================

qt_standard_project_setup(
	I18N_SOURCE_LANGUAGE en_US
	I18N_TRANSLATED_LANGUAGES
)

qt_add_executable(${PROJECT_NAME}
	src/main.cpp
	# src/Launcher.cpp
	src/platform/windows.cpp
	src/DiscordInstall.cpp
	src/LaunchInstance.cpp
	src/LaunchInstanceUpdater.cpp
	src/LaunchInstanceWrapper.cpp
	../shared/shared.cpp
)

if(WIN32)
	target_sources(${PROJECT_NAME} PRIVATE
		resources/app.rc
	)
endif()

set(QML_FILES
	qml/MainWindow.qml
	qml/AddWindow.qml
	qml/PresetRow.qml
	qml/EditWindow.qml
	qml/InstanceItem.qml
	qml/InstanceColumn.qml
	qml/DiscordRow.qml
	qml/Footer.qml
)

foreach (QML_FILE IN LISTS QML_FILES)
	string(REPLACE "qml/" "" TRANSFORMED_PATH "${QML_FILE}")
	message(STATUS "Registering QML file: ${QML_FILE}")
	set_source_files_properties(${QML_FILE} PROPERTIES QT_RESOURCE_ALIAS
			"${TRANSFORMED_PATH}")
endforeach ()

qt_add_qml_module(${PROJECT_NAME}
	URI ${PROJECT_NAME}
	VERSION 1.0

	QML_FILES ${QML_FILES}

	SOURCES
		src/DiscordInstall.hpp
		src/DiscordInstallWrapper.hpp
		src/LaunchInstance.hpp
		src/LaunchInstanceWrapper.hpp

	RESOURCES
		resources/glyphs/add.svg
		resources/glyphs/arrow-upload.svg
		resources/glyphs/copy.svg
		resources/glyphs/delete.svg
		resources/glyphs/dismiss.svg
		resources/glyphs/document-list.svg
		resources/glyphs/edit.svg
		resources/glyphs/folder-arrow-right.svg
		resources/glyphs/folder-open.svg
		resources/glyphs/note.svg
		resources/glyphs/play.svg
		resources/glyphs/settings.svg
		resources/canary.svg
		resources/stable.svg
		resources/development.svg
		resources/icon.ico
		resources/icon.png
		resources/equicord.png
)

qt_add_translations(VencordLauncher TS_FILE_DIR i18n TS_OUTPUT_DIRECTORY i18n)

target_compile_definitions(${PROJECT_NAME} PRIVATE
	VERSION="${VERSION}"
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
	Qt6::Widgets
)

set_target_properties(${PROJECT_NAME} PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

if(APPLE)
	set_target_properties(${PROJECT_NAME} PROPERTIES
					MACOSX_BUNDLE TRUE
	)
endif()

if(MINGW)
	if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
		set_target_properties(${PROJECT_NAME} PROPERTIES
				WIN32_EXECUTABLE TRUE
		)
	endif ()

	target_link_libraries(${PROJECT_NAME} PRIVATE
		lib_detours
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
