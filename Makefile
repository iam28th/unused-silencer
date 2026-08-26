.DEFAULT_GOAL := build

.SHELLFLAGS := -o pipefail -c

# override these variables to compile against built-from-source clang
LLVM_DIR ?= /usr/bin
LLVM_TOOLS_DIR ?= /usr/bin
CLANG_FOR_TESTS ?= $(LLVM_TOOLS_DIR)/clang

PLUGIN := build/lib/libSilencer.so
TOOL := build/bin/silencer

$(PLUGIN): build
$(TOOL): build

.PHONY: conf build clean lit ast query
conf:
		cmake -S . -B build \
		-G 'Unix Makefiles' \
		-DCMAKE_BUILD_TYPE=Debug \
		\
		-DCMAKE_LINKER_TYPE=LLD \
 		-DCMAKE_CXX_COMPILER=clang++ \
		-DCMAKE_C_COMPILER=clang \
		\
		-DCMAKE_PREFIX_PATH=$(LLVM_DIR) \
		-DSILENCER_LLVM_TOOLS_DIR=$(LLVM_TOOLS_DIR)

clean:
	cmake --build build --target clean

JOBS ?= 2  # my thinkpad x220 cannot do more
build:
	cmake --build build -j$(JOBS) --verbose

plugin_name := silencer_plugin
INPUT := tests/plugin/FunctionLocal.cpp

# to modify source code in-place, add:
# -fplugin-arg-$(plugin_name)-inplace
.PHONY: run-plugin
run-plugin: build
	$(CLANG_FOR_TESTS) -c \
		-fdiagnostics-color=always \
		-Xclang -load -Xclang $(PLUGIN) -Xclang -plugin -Xclang $(plugin_name) \
		$(INPUT) 2>&1 | c++filt

.PHONY: run-tool
run-tool: build
	build/bin/silencer $(INPUT) --

ast:
	clang -Xclang -ast-dump -fsyntax-only $(INPUT)

query:
	clang-query -f tests/matcher.cq $(INPUT) --

# --show-all is needed to, well, show commands for all tests,
#  while just -v shows commands only if test fails
lit: FILTER ?= ".*"
lit: build
	lit --show-suites build/tests
	lit -v --show-all --filter $(FILTER) build/tests

.PHONY: lit-tool
lit-tool: build
	lit -v --show-all --filter tool/FunctionLocal.cpp build/tests
