if(APPLE)
	install(TARGETS VencordLauncher BUNDLE DESTINATION .)
else()
	install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
endif()

# what the fuck
if(WIN32 AND MINGW)
	get_filename_component(MINGW_BIN_DIR
		"${CMAKE_CXX_COMPILER}"
		DIRECTORY
	)

	install(FILES
		"${MINGW_BIN_DIR}/libgcc_s_seh-1.dll"
		"${MINGW_BIN_DIR}/libstdc++-6.dll"
		"${MINGW_BIN_DIR}/libwinpthread-1.dll"
		"${MINGW_BIN_DIR}/zlib1.dll"
		"${MINGW_BIN_DIR}/libzstd.dll"
		"${MINGW_BIN_DIR}/libfreetype-6.dll"
		"${MINGW_BIN_DIR}/libbrotlidec.dll"
		"${MINGW_BIN_DIR}/libbrotlicommon.dll"
		"${MINGW_BIN_DIR}/libb2-1.dll"
		"${MINGW_BIN_DIR}/libdouble-conversion.dll"
		"${MINGW_BIN_DIR}/libicuin78.dll"
		"${MINGW_BIN_DIR}/libharfbuzz-0.dll"
		"${MINGW_BIN_DIR}/libmd4c.dll"
		"${MINGW_BIN_DIR}/libpng16-16.dll"
		"${MINGW_BIN_DIR}/libicuuc78.dll"
		"${MINGW_BIN_DIR}/libpcre2-16-0.dll"
		"${MINGW_BIN_DIR}/libicudt78.dll"
		"${MINGW_BIN_DIR}/libglib-2.0-0.dll"
		"${MINGW_BIN_DIR}/libgraphite2.dll"
		"${MINGW_BIN_DIR}/libbz2-1.dll"
		"${MINGW_BIN_DIR}/libintl-8.dll"
		"${MINGW_BIN_DIR}/libpcre2-8-0.dll"
		"${MINGW_BIN_DIR}/libiconv-2.dll"
		DESTINATION bin
	)
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

if (WIN32)
	set(CPACK_GENERATOR "NSIS")
	set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
elseif(APPLE)
	set(CPACK_GENERATOR "DMG")
else()
	set(CPACK_GENERATOR "AppImage;RPM")

	set(CPACK_RPM_PACKAGE_NAME "vencord-launcher")
	set(CPACK_RPM_PACKAGE_SUMMARY "Mod launcher for Discord")
	set(CPACK_RPM_PACKAGE_LICENSE "GPLv3")
	set(CPACK_RPM_PACKAGE_GROUP "Applications/Internet")
	set(CPACK_RPM_PACKAGE_AUTOREQ ON)
	set(CPACK_RPM_PACKAGE_REQUIRES "qt6-qtbase, qt6-qtdeclarative")
endif()

cmake_policy(SET CMP0153 NEW)

include(CPack)
