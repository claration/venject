.PHONY: all clean build package

JOBS		?= 1
BUILD_TYPE	?= Debug

ifeq ($(OS),Windows_NT)
	CMAKE_GENERATOR := -G "MinGW Makefiles"
else
	CMAKE_GENERATOR :=
endif

all: build

configure:
	cmake -S . -B build $(CMAKE_GENERATOR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DQT_DEPLOY_USE_PATCHELF=ON

build: configure
	cmake --build build --config $(BUILD_TYPE) --parallel $(JOBS)

update_translations: configure
	cmake --build build --target update_translations

package: build
	cpack --config build/CPackConfig.cmake -C $(BUILD_TYPE) -DCPACK_PACKAGE_FILE_NAME="venject-windows-x86_64-setup"

clean:
	rm -rf build
