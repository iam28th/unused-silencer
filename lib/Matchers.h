#pragma once

#include <clang/ASTMatchers/ASTMatchers.h>

constexpr static char DeclBinding[] = "decl";
constexpr static char ParentBinding[] = "parent";
constexpr static char FunctionBinding[] = "function";

/// Matches explicit function that has at least 1 parameter, binding it to
/// \c FunctionBinding
extern clang::ast_matchers::DeclarationMatcher FunctionMatcher;
