#include "FixItHints.h"

#include <clang/AST/ASTContext.h>
#include <clang/AST/Decl.h>
#include <clang/AST/Stmt.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Lex/Lexer.h>

using namespace clang;

template <typename NodeType>
static std::string nodeToSourceString(const NodeType *Node, ASTContext *Context)
{
  const auto &SM = Context->getSourceManager();
  const auto &LangOpts = Context->getLangOpts();

  clang::SourceRange Range = Node->getSourceRange();
  clang::CharSourceRange CharRange =
      clang::Lexer::getAsCharRange(Range, SM, LangOpts);

  return std::string{clang::Lexer::getSourceText(CharRange, SM, LangOpts)};
}

FixItHint fixitForUnusedParm(const ParmVarDecl *Parm) {
    auto Name = Parm->getNameAsString();
    SourceLocation Loc = Parm->getLocation();

    const std::string FixItString = (Twine("/*") + Name + ("*/")).str();

    return FixItHint::CreateReplacement(
        SourceRange(Loc, Parm->getEndLoc()), FixItString);
}

std::optional<FixItHint> fixitForDeclStmt(const MatchResult &Result, const DeclStmt *DeclStmt, char endChar, bool addScope) {
  auto *Ctx = Result.Context;

  DeclGroupRef Group = DeclStmt->getDeclGroup();

  std::vector<const VarDecl *> UnusedVars; /// TODO use llvm container

  for (const Decl* Decl : Group) {
    const auto *Var = llvm::dyn_cast<const VarDecl>(Decl);
    if (Var && !Var->isReferenced())
      UnusedVars.push_back(Var);
  }

  if (UnusedVars.empty())
    return std::nullopt;

  std::string FixItString;
  FixItHint Hint{};

  for (const auto *Var : UnusedVars)
    FixItString += (Twine(" (void)") + Var->getName() + Twine(endChar)).str();

  if (addScope) {
    FixItString = (Twine("{ ") + nodeToSourceString(DeclStmt, Ctx) + FixItString + " }").str();
    Hint = FixItHint::CreateReplacement(DeclStmt->getSourceRange(), FixItString);
  } else {
    SourceLocation InsertLoc = Lexer::getLocForEndOfToken(DeclStmt->getEndLoc(), /*offset*/0, Result.Context->getSourceManager(),
      Result.Context->getLangOpts());
    Hint = FixItHint::CreateInsertion(InsertLoc, FixItString);
  }

  assert(!FixItString.empty());
  return Hint;
}
