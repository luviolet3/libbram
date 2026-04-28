default_target: build

debug:
	cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS=-g build
.PHONY: debug

release:
	cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS=-DNDEBUG build
.PHONY: release

configure:
	cmake -B build

build:
	cmake --build build
.PHONY: build

run: build
	./build/tests/ParserTest
.PHONY: run