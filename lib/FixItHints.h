#pragma once

#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/Basic/SourceLocation.h>

using MatchResult = clang::ast_matchers::MatchFinder::MatchResult;

namespace clang {
  class FixItHint;

  class ParmVarDecl;
  class DeclStmt;
}

/// Surrounds \p Parm with comments: /* */
clang::FixItHint fixitForUnusedParm(const clang::ParmVarDecl *Parm);

/// Inserts C-style cast to void for each unused variable delcaration in \p DeclStmt, optionally enclosing it in a new scope.
///
/// \p DeclStmt Statement to check and fix
///
/// \p EndChar Character to insert at the end of the fixit;
///            ',' should be used for initialization in If and similar, and ';' in other cases
///
/// \return Genrated FixItHint, or std::nullopt if this \p DeclStmt is fine
std::optional<clang::FixItHint> fixitForDeclStmt(const MatchResult &Result, const clang::DeclStmt *DeclStmt, char endChar, bool addScope);
