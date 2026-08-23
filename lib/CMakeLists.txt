message(DEBUG "LLVM include dirs: " ${LLVM_INCLUDE_DIRS})
message(DEBUG "CLANG include dirs: " ${CLANG_INCLUDE_DIRS})

add_library(${SILENCER_PLUGIN_SONAME} SHARED
  PluginActionShared.cpp
	Silencer.cpp
)
target_include_directories(${SILENCER_PLUGIN_SONAME}
	PRIVATE "${CMAKE_SOURCE_DIR}/include"
)
target_include_directories(${SILENCER_PLUGIN_SONAME} SYSTEM
	PRIVATE ${CLANG_INCLUDE_DIRS}
	        ${LLVM_INCLUDE_DIRS}
)

set(STATICLIB ${SILENCER_PLUGIN_SONAME}_STATIC)
add_library(${STATICLIB}
  PluginActionShared.cpp
	Silencer.cpp
)
target_include_directories(${STATICLIB}
  PUBLIC "${CMAKE_SOURCE_DIR}/include"
)
target_include_directories(${STATICLIB} SYSTEM
	PRIVATE ${CLANG_INCLUDE_DIRS}
	        ${LLVM_INCLUDE_DIRS}
)
