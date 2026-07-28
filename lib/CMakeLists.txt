add_library(${SILENCER_PLUGIN_SONAME} SHARED
	Silencer.cpp
)

target_include_directories(${SILENCER_PLUGIN_SONAME}
	PRIVATE "${CMAKE_SOURCE_DIR}/include"
)

message(DEBUG "LLVM include dirs: " ${LLVM_INCLUDE_DIRS})
message(DEBUG "CLANG include dirs: " ${CLANG_INCLUDE_DIRS})

target_include_directories(${SILENCER_PLUGIN_SONAME} SYSTEM
	PRIVATE ${CLANG_INCLUDE_DIRS}
	        ${LLVM_INCLUDE_DIRS}
)
