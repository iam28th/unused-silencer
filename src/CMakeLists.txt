add_library(${DICTCHECK_PLUGIN_SONAME} SHARED
	Silencer.cpp
	Silencer.h
)

message(DEBUG "LLVM include dirs: " ${LLVM_INCLUDE_DIRS})
message(DEBUG "CLANG include dirs: " ${CLANG_INCLUDE_DIRS})

target_include_directories(${DICTCHECK_PLUGIN_SONAME} SYSTEM
	PRIVATE ${CLANG_INCLUDE_DIRS}
	        ${LLVM_INCLUDE_DIRS}
)
