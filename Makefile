.DEFAULT_GOAL := build

LLVM_DIR ?= ../llvm-project/build
LLVM_TOOLS_DIR ?= $(shell readlink -f ../llvm-project/build/bin)
CLANG ?= $(LLVM_TOOLS_DIR)/clang

PLUGIN := build/lib/libSilencerPlugin_AstVisitor.so

$(PLUGIN): build

.PHONY: conf build run run-cc1 lit
conf:
	CMAKE_BUILD_TYPE=Debug \
		cmake -S . -B build \
		-G Ninja \
		-DCMAKE_LINKER_TYPE=LLD \
		-DCMAKE_PREFIX_PATH=$(LLVM_DIR) \
		-DSILENCER_LLVM_TOOLS_DIR=$(LLVM_TOOLS_DIR)

build: conf
	cmake --build build -j2 --verbose


plugin_name := "silencer"
input := tests/Basic.cpp

run: build
	$(CLANG) -c \
		-Xclang -load -Xclang $(PLUGIN) -Xclang -plugin -Xclang $(plugin_name)
		$(input)

run-cc1: build
	$(CLANG) -cc1 \
		-load $(PLUGIN) -plugin $(plugin_name)
		-fcolor-diagnostics \
		$(input)

# TODO:
# run-tool (make it a standalone binary)

# --show-all is needed to, well, show commands for all tests,
#  while just -v shows commands only if test fails
lit: build
	lit --show-suites build/tests
	lit -v --show-all build/tests
