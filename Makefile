default_target: build

debug:
	cmake -DCMAKE_BUILD_TYPE=Debug build
.PHONY: debug

release:
	cmake -DCMAKE_BUILD_TYPE=Release build
.PHONY: release

configure:
	cmake -B build

build: configure
	cmake --build build
.PHONY: build

run: build
	./build/tests/ParserTest
.PHONY: run