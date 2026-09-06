message(DEBUG "LLVM include dirs: " ${LLVM_INCLUDE_DIRS})
message(DEBUG "CLANG include dirs: " ${CLANG_INCLUDE_DIRS})

set(SILENCER_SOURCES
  PluginActionShared.cpp
  Silencer.cpp
  Matchers.cpp
  Matchers.h
)

add_library(${SILENCER_PLUGIN_SONAME} SHARED
  ${SILENCER_SOURCES}
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
  ${SILENCER_SOURCES}
)

target_include_directories(${STATICLIB}
  PUBLIC "${CMAKE_SOURCE_DIR}/include"
)

target_include_directories(${STATICLIB} SYSTEM
	PRIVATE ${CLANG_INCLUDE_DIRS}
	        ${LLVM_INCLUDE_DIRS}
)
