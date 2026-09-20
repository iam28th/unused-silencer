#include "Silencer.h"

#include "FixItHints.h"
#include "Matchers.h"

#include <clang/AST/Decl.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Frontend/FrontendPluginRegistry.h>
#include <clang/Lex/Lexer.h>
#include <llvm/Support/Casting.h>
#include <llvm/Support/raw_ostream.h>

using namespace clang;
using namespace ast_matchers;

using MatchResult = clang::ast_matchers::MatchFinder::MatchResult;

void ParamHandler::run(const MatchResult &Result) {
  ASTContext *Ctx = Result.Context;

  const FunctionDecl *Fn = Result.Nodes.getNodeAs<clang::FunctionDecl>(FunctionBinding);
  assert(Fn && "nullptr in matcher callback");

  const int N = Fn->getNumParams();
  for (int i = 0; i < N; ++i) {
    const ParmVarDecl *Parm = Fn->getParamDecl(i);
    if (Parm->isUsed())
      continue;

    auto FixItHint = fixitForUnusedParm(Parm);

    DiagnosticsEngine &DiagEngine = Ctx->getDiagnostics();
    unsigned DiagID = DiagEngine.getCustomDiagID(DiagnosticsEngine::Warning,
                                                 "unused argument");
    DiagEngine.Report(Parm->getLocation(), DiagID).AddFixItHint(FixItHint);
    Rewriter.ReplaceText(FixItHint.RemoveRange, FixItHint.CodeToInsert);
  }
}

template <typename NodeType>
std::string nodeToSourceString(const NodeType *Node, ASTContext *Context)
{
  const auto &SM = Context->getSourceManager();
  const auto &LangOpts = Context->getLangOpts();

  clang::SourceRange Range = Node->getSourceRange();
  clang::CharSourceRange CharRange =
      clang::Lexer::getAsCharRange(Range, SM, LangOpts);

  return std::string{clang::Lexer::getSourceText(CharRange, SM, LangOpts)};
}

std::optional<FixItHint> fixitForLocalVar(const MatchResult &Result, const DeclStmt *DeclStmt, char endChar, bool addScope) {
  auto *Ctx = Result.Context;

  llvm::errs() << nodeToSourceString(DeclStmt, Ctx) << '\n';
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

  return Hint;

  assert(!FixItString.empty());

}

void DeclarationStmtHandler::run(const MatchResult &Result) {
  const auto &SM = Result.Context->getSourceManager();
  DiagnosticsEngine &DiagEngine = Result.Context->getDiagnostics();
  unsigned DiagID =
    DiagEngine.getCustomDiagID(DiagnosticsEngine::Warning, "unused variable(s)");

  const DeclStmt *Decl = Result.Nodes.getNodeAs<clang::DeclStmt>("decl");
  const Stmt *Parent = Result.Nodes.getNodeAs<clang::Stmt>("parent");
  assert(Decl && Parent);

  if (llvm::dyn_cast<CompoundStmt>(Parent)) {
    // parent is compund;
    // simple case, can be handled by inserting '(void)x;' statement for each unused x
    auto Hint = fixitForLocalVar(Result, Decl, /*endChar*/';', /*addScope*/false);

    if (Hint.has_value()) {
      DiagEngine.Report(Decl->getBeginLoc(), DiagID).AddFixItHint(*Hint);
    }
    return;
  }

  if (const IfStmt *If = llvm::dyn_cast<IfStmt>(Parent)) {
    // parent is if
    const Stmt *Init = If->getInit();

    if (Init != static_cast<const Stmt *>(Decl)) {
      // we're in the body
      // same as above, but since we're adding more statements, we need to enclode entire result in a new scope
      auto Hint = fixitForLocalVar(Result, Decl, /*endChar*/ ';', /*addScope*/true);
      if (Hint.has_value()) {
        DiagEngine.Report(Decl->getBeginLoc(), DiagID).AddFixItHint(*Hint);
      }
      return;
    }

    // we're in initialization...
    // need to get condition exprssion, and prepend it with '(void)x,' for each x
    // we can reuse function that appends stamements after original declGroup,
    // but end each one with a coma instead of semicolon
    // (this is a bit of hack though, but it seems to work)
    auto Hint = fixitForLocalVar(Result, Decl, /*endChar*/ ',', /*addScope*/false);
    if (Hint.has_value()) {
      DiagEngine.Report(Decl->getBeginLoc(), DiagID).AddFixItHint(*Hint);
    }
    return;
  }

  // TODO handle while, switch & for (should be pretty much same as if)
  llvm::errs() << "Unsupported case";
}

/// Handles case when creating a new scope is not required for insertion
///
/// \param inIf true if match is in the beginning of if statement (when we need
/// to end insertion with comma)
static void handleDeclarationWithoutNewScope(const MatchResult &Result,
                                             bool inIf,
                                             clang::Rewriter &Rewriter) {
  const VarDecl *Var = Result.Nodes.getNodeAs<clang::VarDecl>(VarBinding);
  assert(Var && "nullptr in matcher callback");
  if (Var->isReferenced())
    return;

  const DeclStmt *Parent =
      Result.Nodes.getNodeAs<clang::DeclStmt>(ParentBinding);
  assert(Parent && "parent can't be null");

  SourceLocation InsertLoc = insertLocForUnusedLocalVarWithoutNewScopeFixit(Var, Parent, Result.Context);
  FixItHint Hint = fixitForUnusedLocalVarWithoutNewScope(Var, /*EndChar*/ (inIf ? ',' : ';'), InsertLoc);

  DiagnosticsEngine &DiagEngine = Result.Context->getDiagnostics();
  unsigned DiagID =
      DiagEngine.getCustomDiagID(DiagnosticsEngine::Warning, "unused variable");
  DiagEngine.Report(Var->getLocation(), DiagID).AddFixItHint(Hint);
  Rewriter.InsertText(InsertLoc, Hint.CodeToInsert);
}

/// Handles case when we need to insert a new scope around declaration
/// Replaces 'auto x = 0' with '{ auto x = 0; (void) x; }'
///
/// Currently this works only with single decl
static void handleDeclarationWithNewScope(const MatchResult &Result,
                                          clang::Rewriter &Rewriter) {
  const VarDecl *Var = Result.Nodes.getNodeAs<clang::VarDecl>(VarBinding);
  assert(Var && "nullptr in matcher callback");
  if (Var->isReferenced())
    return;

  const DeclStmt *Parent =
      Result.Nodes.getNodeAs<clang::DeclStmt>(ParentBinding);
  assert(Parent && "parent can't be null");

  ASTContext *Ctx = Result.Context;
  const auto &SM = Ctx->getSourceManager();

  FixItHint Hint = fixitForUnusedLocalVarWithNewScope(Var, Parent, Result.Context);

  DiagnosticsEngine &DiagEngine = Ctx->getDiagnostics();
  unsigned DiagID =
      DiagEngine.getCustomDiagID(DiagnosticsEngine::Warning, "unused variable");
  DiagEngine.Report(Var->getLocation(), DiagID).AddFixItHint(Hint);

  Rewriter.ReplaceText(Hint.RemoveRange, Hint.CodeToInsert);
}

void CompoundStmtVarHandler::run(
    const clang::ast_matchers::MatchFinder::MatchResult &Result) {
  handleDeclarationWithoutNewScope(Result, /*inIf*/ false, Rewriter);
}

void IfStmtVarHandler::run(const MatchResult &Result) {
  const IfStmt *If = Result.Nodes.getNodeAs<clang::IfStmt>(IfBinding);
  If->getInit();

  const DeclStmt *Parent =
      Result.Nodes.getNodeAs<clang::DeclStmt>(ParentBinding);
  assert(If && "nullptr in matcher callback");
  assert(Parent && "nullptr in matcher callback");

  if (If->getInit() == llvm::dyn_cast<Stmt>(Parent))
    handleDeclarationWithoutNewScope(Result, /*inIf*/ true, Rewriter);
  else
    handleDeclarationWithNewScope(Result, Rewriter);
}

SilencerASTConsumer::SilencerASTConsumer(clang::Rewriter &Rewriter)
    : Rewriter(Rewriter), DeclarationStmtHandler(Rewriter), ParamHandler(Rewriter), IfStmtVarHandler(Rewriter),
      CompoundStmtVarHandler(Rewriter) {
  // Finder.addMatcher(VarInIf, &IfStmtVarHandler);
  // Finder.addMatcher(VarInCompound, &CompoundStmtVarHandler);
  Finder.addMatcher(FunctionMatcher, &ParamHandler);

  Finder.addMatcher(
      declStmt(
        hasParent(stmt().bind("parent"))
      ).bind("decl"), &DeclarationStmtHandler);
}

void SilencerASTConsumer::HandleTranslationUnit(clang::ASTContext &Ctx) {
  Finder.matchAST(Ctx);
}

bool SilencerPluginAction_Plugin::ParseArgs(
    const CompilerInstance &, const std::vector<std::string> &args) {
  for (const auto &arg : args) {
    if (arg == "inplace") {
      ModifyInputInplace = true;
    }
  }
  return true;
}

void SilencerPluginAction_Plugin::EndSourceFileAction() {
  if (ModifyInputInplace) {
    Rewriter.overwriteChangedFiles();
  } else {
    Rewriter.getEditBuffer(Rewriter.getSourceMgr().getMainFileID())
        .write(llvm::outs());
  }
}

static FrontendPluginRegistry::Add<SilencerPluginAction_Plugin>
    X(/*Name=*/"silencer_plugin",
      /*Desc=*/"Rewrites code so that there's no warning about unused local "
               "variables or function args");
