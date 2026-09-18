#pragma once

#include <clang/Basic/SourceLocation.h>

namespace clang {
  class ASTContext;
  class FixItHint;

  class ParmVarDecl;
  class VarDecl;
  class DeclStmt;
}

/// Surrounds \p Parm with comments: /* */
clang::FixItHint fixitForUnusedParm(const clang::ParmVarDecl *Parm);

/// Inserts C-style cast to void for \p Var either as a new statement, using comma operator.
///
/// \p EndChar Character to insert at the end of the fixit;
///                in can be either ',' or ';' depending on context
/// \p InsertLocation Where to insert C-style cast. Passed as parameter, because its calculation is non-trivial..
clang::FixItHint fixitForUnusedLocalVarWithoutNewScope(const clang::VarDecl *Var, char EndChar, clang::SourceLocation InsertLoc);

/// @return Where to insert "(void)name;"
clang::SourceLocation insertLocForUnusedLocalVarWithoutNewScopeFixit(const clang::VarDecl *Var,
    const clang::DeclStmt *Parent, clang::ASTContext *Ctx);

/// Currently this works only with single decl
/// TODO handle case with declgroup/operator
clang::FixItHint fixitForUnusedLocalVarWithNewScope(const clang::VarDecl *Var,
    const clang::DeclStmt *Parent, clang::ASTContext *Ctx);
