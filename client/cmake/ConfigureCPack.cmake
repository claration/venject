install(TARGETS samaraqt RUNTIME DESTINATION bin)

# include(InstallRequiredSystemLibraries)

cmake_policy(SET CMP0153 NEW)

qt_generate_deploy_qml_app_script(
    TARGET samaraqt
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

	POST_EXCLUDE_REGEXES
        "libc\\.so.*"
        "libm\\.so.*"
        "libpthread\\.so.*"
        "libdl\\.so.*"
        "librt\\.so.*"
        "libresolv\\.so.*"
)

install(SCRIPT ${DEPLOY_SCRIPT})

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

set(CPACK_PACKAGE_NAME "Vencord Launcher")
set(CPACK_PACKAGE_VENDOR "C")
set(CPACK_PACKAGE_DESCRIPTION "Something cool!")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_EXECUTABLES samaraqt)
set(CPACK_PACKAGE_INSTALL_DIRECTORY "VencordLauncher")
set(CPACK_GENERATOR "TGZ;AppImage")
set(CPACK_APPIMAGE_DESKTOP_FILE "on.clarati.VencordLauncher.desktop")
set(CPACK_PACKAGE_ICON "on.clarati.VencordLauncher.png")
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}")

include(CPack)
