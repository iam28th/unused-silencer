#pragma once

#include <clang/ASTMatchers/ASTMatchers.h>

constexpr static char IfBinding[] = "if";
constexpr static char FunctionBinding[] = "fn";
constexpr static char VarBinding[] = "var";
constexpr static char ParentBinding[] = "parent";

/// Matches explicit function that has at least 1 parameter, binding it to
/// \c FunctionBinding
extern clang::ast_matchers::DeclarationMatcher FunctionMatcher;

/// Matches variable that is declared in if statement, either in condition or in body.
/// Binds variable to \c VarBinding, its parent to \c ParentBinding, and if statement to \c IfBinding.
///
/// If statement might or might not be variable's parent
extern clang::ast_matchers::DeclarationMatcher VarInIf;

/// Matches 'local' variable declared in a compound statement,
/// excluding function parameters and cases covered by VarInIf
extern clang::ast_matchers::DeclarationMatcher VarInCompound;
