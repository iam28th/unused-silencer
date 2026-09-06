#include "Matchers.h"

using namespace clang::ast_matchers;

// clang-format off
DeclarationMatcher FunctionMatcher =
    functionDecl(
      isDefinition(),
      unless(isImplicit()),
      hasAnyParameter(anything()))
   .bind("fn");

DeclarationMatcher VarInIf =
  varDecl(
    hasParent(
      declStmt(hasParent(ifStmt().bind(IfBinding))
    ).bind(ParentBinding))
  ).bind(VarBinding);

DeclarationMatcher VarInCompound =
  varDecl(
    unless(VarInIf),
    unless(parmVarDecl()),
    isDefinition(),
    hasAncestor(functionDecl(isDefinition())),
    hasParent(declStmt().bind(ParentBinding))
  ).bind(VarBinding);
// clang-format on
