if(APPLE)
	install(TARGETS VencordLauncher BUNDLE DESTINATION .)
else()
	install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
endif()

# what the fuck
if(WIN32 AND MINGW)
	get_filename_component(MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)

	if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		set(COMPILER_DLLS libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll)
	elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
		set(COMPILER_DLLS libc++.dll libunwind.dll)
	endif()

	set(ALL_DLL_NAMES
		${COMPILER_DLLS}
		zlib1.dll
		libzstd.dll
		libfreetype-6.dll
		libbrotlidec.dll
		libbrotlicommon.dll
		libb2-1.dll
		libdouble-conversion.dll
		libicuin78.dll
		libharfbuzz-0.dll
		libmd4c.dll
		libpng16-16.dll
		libicuuc78.dll
		libpcre2-16-0.dll
		libicudt78.dll
		libglib-2.0-0.dll
		libgraphite2.dll
		libbz2-1.dll
		libintl-8.dll
		libpcre2-8-0.dll
		libiconv-2.dll
	)

	foreach(dll ${ALL_DLL_NAMES})
		list(APPEND INSTALL_FILES "${MINGW_BIN_DIR}/${dll}")
	endforeach()

	install(FILES ${INSTALL_FILES} DESTINATION bin)
endif()

if(ENABLE_QT_DEPLOY)
	set(include_plugins "")
	if(LINUX)
		set(include_plugins INCLUDE_PLUGINS wayland xcb)
	endif()

	qt_generate_deploy_qml_app_script(
		TARGET ${PROJECT_NAME}
		OUTPUT_SCRIPT DEPLOY_SCRIPT

		EXCLUDE_PLUGINS
			kimg_ani
			kimg_avif
			kimg_dds
			kimg_eps
			kimg_exr
			kimg_ff
			kimg_hdr
			kimg_heif
			kimg_iff
			kimg_jp2
			kimg_jxl
			kimg_jxr
			kimg_kra
			kimg_pcx
			kimg_pfm
			kimg_pic
			kimg_psd
			kimg_pxr
			kimg_qoi
			kimg_ras
			kimg_raw
			kimg_rgb
			kimg_sct
			kimg_tga
			kimg_tim
			kimg_xcf

			KDEPlasmaPlatformTheme6
			layer-shell

		${include_plugins}

		POST_EXCLUDE_REGEXES
			"libc\\.so.*"
			"libm\\.so.*"
			"libpthread\\.so.*"
			"libdl\\.so.*"
			"librt\\.so.*"
			"libresolv\\.so.*"
	)

	install(SCRIPT ${DEPLOY_SCRIPT})
endif()

if (NOT WIN32)
	install(
		FILES "${CMAKE_CURRENT_SOURCE_DIR}/resources/on.clarati.VencordLauncher.desktop"
		DESTINATION share/applications
	)

	install(
		FILES "${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/hicolor/64x64/apps/on.clarati.VencordLauncher.png"
		DESTINATION share/icons/hicolor/64x64/apps
	)

	install(
		FILES "${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/hicolor/128x128/apps/on.clarati.VencordLauncher.png"
		DESTINATION share/icons/hicolor/128x128/apps
	)

	install(
		FILES "${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/hicolor/256x256/apps/on.clarati.VencordLauncher.png"
		DESTINATION share/icons/hicolor/256x256/apps
	)

	install(
		FILES "${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/hicolor/512x512/apps/on.clarati.VencordLauncher.png"
		DESTINATION share/icons/hicolor/512x512/apps
	)
endif()

set(CPACK_PACKAGE_NAME "VencordLauncher")
set(CPACK_PACKAGE_VENDOR "C")
set(CPACK_PACKAGE_DESCRIPTION "Mod launcher for Discord")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "VencordLauncher")
set(CPACK_APPIMAGE_DESKTOP_FILE "on.clarati.VencordLauncher.desktop")

if (NOT WIN32)
	set(CPACK_PACKAGE_ICON "on.clarati.VencordLauncher.png")
endif()

set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CPACK_PACKAGE_EXECUTABLES "VencordLauncher;Vencord Launcher")
set(CPACK_PACKAGE_VERSION ${VERSION})

if (WIN32)
	set(CPACK_GENERATOR "NSIS")
	set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
elseif(APPLE)
	set(CPACK_GENERATOR "DMG")
else()
	set(CPACK_GENERATOR "AppImage;RPM;DEB")

	# RPM
	set(CPACK_RPM_PACKAGE_NAME "vencord-launcher")
	set(CPACK_RPM_PACKAGE_SUMMARY "Launcher for Discord which loads Vencord")
	set(CPACK_RPM_PACKAGE_LICENSE "GPLv3")
	set(CPACK_RPM_PACKAGE_GROUP "Applications/Internet")
	set(CPACK_RPM_PACKAGE_AUTOREQ ON)
	set(CPACK_RPM_PACKAGE_REQUIRES
		"qt6-qtbase, qt6-qtdeclarative, qt6-qtquickcontrols2"
	)

	# DEB
	set(CPACK_DEBIAN_PACKAGE_NAME "vencord-launcher")
	set(CPACK_DEBIAN_PACKAGE_DESCRIPTION
		"Launcher for Discord which loads Vencord"
	)
	set(CPACK_DEBIAN_PACKAGE_MAINTAINER "C <claration@riseup.net>")
	set(CPACK_DEBIAN_PACKAGE_SECTION "net")
	set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
	set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
	set(CPACK_DEBIAN_PACKAGE_DEPENDS
		"libqt6core6, libqt6qml6, libqt63dquick6"
	)
endif()

cmake_policy(SET CMP0153 NEW)

include(CPack)
