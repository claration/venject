.PHONY: all clean build package configure update_translations

JOBS						?= 1
BUILD_TYPE					?= Debug
ARCH 						:= $(shell uname -m)

PACKAGE_GENERATOR			?= AppImage
ENABLE_QT_DEPLOY			?= ON

ifeq ($(OS),Windows_NT)
	CMAKE_GENERATOR 		:= -G "MinGW Makefiles"
	CPACK_PACKAGE_FILE_NAME := "VencordLauncher-windows-$(ARCH)-setup"
else
	CMAKE_GENERATOR 		:=
	CPACK_PACKAGE_FILE_NAME := "VencordLauncher-linux-$(ARCH)"
endif

all: build

configure:
	cmake -S . -B build $(CMAKE_GENERATOR) \
		-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
		-DQT_DEPLOY_USE_PATCHELF=ON \
		-DENABLE_QT_DEPLOY=$(ENABLE_QT_DEPLOY)

build: configure
	cmake --build build --config $(BUILD_TYPE) --parallel $(JOBS)

update_translations: configure
	cmake --build build --target update_translations

package: configure
	cpack --config build/CPackConfig.cmake \
		-G $(PACKAGE_GENERATOR) \
		-DCPACK_PACKAGE_FILE_NAME=$(CPACK_PACKAGE_FILE_NAME)

clean:
	rm -rf build
