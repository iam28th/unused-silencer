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

void DeclarationStmtHandler::run(const MatchResult &Result) {
  const auto &SM = Result.Context->getSourceManager();
  DiagnosticsEngine &DiagEngine = Result.Context->getDiagnostics();
  unsigned DiagID =
    DiagEngine.getCustomDiagID(DiagnosticsEngine::Warning, "unused variable(s)");

  const DeclStmt *Decl = Result.Nodes.getNodeAs<clang::DeclStmt>(DeclBinding);
  const Stmt *Parent = Result.Nodes.getNodeAs<clang::Stmt>(ParentBinding);
  assert(Decl && Parent);

  // TODO handle initializer in while, switch & for (should be pretty much same as if)
  std::optional<FixItHint> Hint;

  if (llvm::dyn_cast<CompoundStmt>(Parent)) {
    // parent is compund;
    // simple case, can be handled by inserting '(void)x;' statement for each unused x
    Hint = fixitForDeclStmt(Result, Decl, /*endChar*/';', /*addScope*/false);
  } else if (const IfStmt *If = llvm::dyn_cast<IfStmt>(Parent)) {
    // parent is if
    const Stmt *Init = If->getInit();

    if (Init != static_cast<const Stmt *>(Decl)) {
      // we're in the body
      // same as above, but since we're adding more statements, we need to enclode entire result in a new scope
      Hint = fixitForDeclStmt(Result, Decl, /*endChar*/ ';', /*addScope*/true);
    } else {
      // we're in initialization...
      // need to get condition exprssion, and prepend it with '(void)x,' for each x
      // we can reuse function that appends stamements after original declGroup,
      // but end each one with a coma instead of semicolon
      // (this is a bit of hack though, but it seems to work)
      Hint = fixitForDeclStmt(Result, Decl, /*endChar*/ ',', /*addScope*/false);
    }
  } else {
    llvm::errs() << "Unsupported case";
    return;
  }

  if (!Hint.has_value())
    return;

  DiagEngine.Report(Decl->getBeginLoc(), DiagID).AddFixItHint(*Hint);
  Rewriter.ReplaceText(Hint->RemoveRange, Hint->CodeToInsert);
}


SilencerASTConsumer::SilencerASTConsumer(clang::Rewriter &Rewriter)
    : Rewriter(Rewriter), DeclarationStmtHandler(Rewriter), ParamHandler(Rewriter) {
  Finder.addMatcher(FunctionMatcher, &ParamHandler);

  Finder.addMatcher(
    declStmt(
      hasParent(stmt().bind(ParentBinding))
    ).bind("decl"), &DeclarationStmtHandler);
}

void SilencerASTConsumer::HandleTranslationUnit(clang::ASTContext &Ctx) {
  Finder.matchAST(Ctx);
}

bool SilencerPluginAction_Plugin::ParseArgs(
    const CompilerInstance &, const std::vector<std::string> &args) {
  for (const auto &arg : args) {
    if (arg == "inplace")
      ModifyInputInplace = true;
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
