#include "FixItHints.h"

#include <clang/AST/ASTContext.h>
#include <clang/AST/Decl.h>
#include <clang/AST/Stmt.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Lex/Lexer.h>

using namespace clang;

FixItHint fixitForUnusedParm(const ParmVarDecl *Parm) {
    auto Name = Parm->getNameAsString();
    SourceLocation Loc = Parm->getLocation();

    const std::string FixItString = (Twine("/*") + Name + ("*/")).str();

    return FixItHint::CreateReplacement(
        SourceRange(Loc, Parm->getEndLoc()), FixItString);
}

FixItHint fixitForUnusedLocalVarWithoutNewScope(const VarDecl *Var, char EndChar, SourceLocation InsertLoc) {
  auto Name = Var->getNameAsString();

  const std::string SuggestedFixString =
      (Twine(" (void)") + Name + Twine(EndChar)).str();
  return FixItHint::CreateInsertion(InsertLoc, SuggestedFixString);
}

SourceLocation insertLocForUnusedLocalVarWithoutNewScopeFixit(const VarDecl *Var,
    const DeclStmt *Parent, ASTContext *Ctx)
{
  return Lexer::getLocForEndOfToken(Parent->getEndLoc(), /*offset*/0, Ctx->getSourceManager(),
      Ctx->getLangOpts());
}

FixItHint fixitForUnusedLocalVarWithNewScope(const VarDecl *Var,
    const DeclStmt *Parent, clang::ASTContext *Ctx) {
  const auto &SM = Ctx->getSourceManager();

  const std::string DeclString{SM.getCharacterData(Parent->getBeginLoc()),
                               SM.getCharacterData(Parent->getEndLoc())};

  const std::string SuggestedFixString =
      (Twine("{ ") + DeclString + "; (void)" + Var->getNameAsString() + "; }")
          .str();

  return FixItHint::CreateReplacement(Parent->getSourceRange(), SuggestedFixString);
}
