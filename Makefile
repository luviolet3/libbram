default_target: debug

debug:
	cmake -DCMAKE_BUILD_TYPE=Debug build
	cmake --build build
.PHONY: debug

release:
	cmake -DCMAKE_BUILD_TYPE=Release build
	cmake --build build
.PHONY: release

run:
	./build/ParserTest
.PHONY: run