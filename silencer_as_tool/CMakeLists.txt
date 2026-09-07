add_executable(silencer
  silencer_main.cpp
)

# see /usr/lib/cmake/clang/ClangConfig.cmake for target names
target_link_libraries(silencer
  ${SILENCER_PLUGIN_SONAME}_STATIC
  LLVM
  clang
  clang-cpp
)
