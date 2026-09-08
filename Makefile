.PHONY: all clean build package

JOBS       ?= 1
BUILD_TYPE ?= Debug

ifeq ($(OS),Windows_NT)
    CMAKE_GENERATOR := -G "MinGW Makefiles"
else
    CMAKE_GENERATOR :=
endif

all: build

build:
	cmake -S . -B build $(CMAKE_GENERATOR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
	cmake --build build --config $(BUILD_TYPE) --parallel $(JOBS)

package: build
	cpack --config build/CPackConfig.cmake -C $(BUILD_TYPE)

clean:
	$(MAKE) -C venject clean
	$(MAKE) -C venject/scripts clean
	rm -rf build