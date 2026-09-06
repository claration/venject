.PHONY: all clean build package

JOBS 		?= 1
BUILD_TYPE 	?= Debug

all: build

build:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
	cmake --build build --parallel $(JOBS)

package: build
	cpack --config build/CPackConfig.cmake

clean:
	$(MAKE) -C venject clean
	$(MAKE) -C venject/scripts clean
	rm -rf build
