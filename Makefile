.DEFAULT_GOAL := build

.SHELLFLAGS := -o pipefail -c

LLVM_DIR ?= ../llvm-project/build
LLVM_TOOLS_DIR ?= $(shell readlink -f ../llvm-project/build/bin)
CLANG_FOR_TESTS ?= $(LLVM_TOOLS_DIR)/clang

PLUGIN := build/lib/libSilencer.so

$(PLUGIN): build

.PHONY: conf build clean run run-cc1 lit ast query
conf:
		cmake -S . -B build \
		-G 'Unix Makefiles' \
		-DCMAKE_BUILD_TYPE=Debug \
		\
		-DCMAKE_LINKER_TYPE=LLD \
 		-DCMAKE_CXX_COMPILER=$(shell which clang) \
		-DCMAKE_C_COMPILER=$(shell which clang) \
		\
		-DCMAKE_PREFIX_PATH=$(LLVM_DIR) \
		-DSILENCER_LLVM_TOOLS_DIR=$(LLVM_TOOLS_DIR)

clean:
	cmake --build build --target clean

build: conf
	cmake --build build -j2 --verbose

plugin_name := silencer
input := tests/Basic.cpp

# run: build
# 	$(CLANG_FOR_TESTS) -c \
# 		-fdiagnostics-color=always \
# 		-Xclang -fdiagnostics-parseable-fixits \
# 		-Xclang -load -Xclang $(PLUGIN) -Xclang -plugin -Xclang $(plugin_name) \
# 		-Wreorder \
# 		-Xclang -fixit \
# 		$(input) 2>&1 | c++filt

run: build
	$(CLANG_FOR_TESTS) -c \
		-fdiagnostics-color=always \
		-Xclang -load -Xclang $(PLUGIN) -Xclang -plugin -Xclang $(plugin_name) \
		$(input) 2>&1 | c++filt

# -fplugin-arg-$(plugin_name)-inplace \

# -Xclang -load -Xclang $(PLUGIN) -Xclang -plugin -Xclang $(plugin_name) \

ast:
	clang -Xclang -ast-dump -fsyntax-only $(input)

query:
	clang-query -f tests/matcher.cq $(input) --

# alternative ways to launch plugin
run-cc1: build
	$(CLANG_FOR_TESTS) -cc1 \
		-load $(PLUGIN) -plugin $(plugin_name) \
		-fcolor-diagnostics \
		-fixit \
		$(input)

# --show-all is needed to, well, show commands for all tests,
#  while just -v shows commands only if test fails
lit: build
	lit --show-suites build/tests
	lit -v --show-all build/tests
