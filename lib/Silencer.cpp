#include "Silencer.h"

#include <clang/AST/Decl.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Frontend/FrontendPluginRegistry.h>
#include <llvm/Support/raw_ostream.h>

using namespace clang;
using namespace ast_matchers;

void ParamHandler::run(
    const clang::ast_matchers::MatchFinder::MatchResult &Result) {

  /// Print name and location of the argument
  ASTContext *Ctx = Result.Context;

  const FunctionDecl *Fn = Result.Nodes.getNodeAs<clang::FunctionDecl>("fn");
  assert(Fn && "nullptr in matcher callback");

  auto &SourceMgr = Rewriter.getSourceMgr();

  const int N = Fn->getNumParams();
  for (int i = 0; i < N; ++i) {
    const ParmVarDecl *Parm = Fn->getParamDecl(i);
    if (Parm->isUsed())
      continue;

    SourceLocation ParmLoc = Parm->getLocation();

    // construct fixit...
    auto Name = Parm->getNameAsString();
    const std::string FixItString = (Twine("/*") + Name + ("*/")).str();

    FixItHint FixItHint = FixItHint::CreateReplacement(
        SourceRange(ParmLoc, ParmLoc.getLocWithOffset(Name.size())),
        FixItString);

    DiagnosticsEngine &DiagEngine = Ctx->getDiagnostics();
    unsigned DiagID = DiagEngine.getCustomDiagID(DiagnosticsEngine::Warning,
                                                 "unused argument");
    DiagEngine.Report(Parm->getLocation(), DiagID).AddFixItHint(FixItHint);
    Rewriter.ReplaceText(ParmLoc, Name.length(), FixItString);
  }
}

void ParamHandler::onEndOfTranslationUnit() {}

void LocalVarHandler::run(
    const clang::ast_matchers::MatchFinder::MatchResult &) {}
void LocalVarHandler::onEndOfTranslationUnit() {}

SilencerASTConsumer::SilencerASTConsumer(clang::Rewriter &Rewriter)
    : Rewriter(Rewriter), ParamHandler(Rewriter), LocalVarHandler(Rewriter) {
  DeclarationMatcher FunctionMatcher =
      functionDecl(isDefinition(), unless(isImplicit()),
                   hasAnyParameter(anything()))
          .bind("fn");
  Finder.addMatcher(FunctionMatcher, &ParamHandler);
#if 0
  DeclarationMatcher LocVarMatcher =
      varDecl(unless(parmVarDecl()), isDefinition(),
              hasAncestor(functionDecl(isDefinition())))
          .bind("var");
  Finder.addMatcher(LocVarMatcher, &LocalVarHandler);
#endif
}

void SilencerASTConsumer::HandleTranslationUnit(clang::ASTContext &Ctx) {
  Finder.matchAST(Ctx);
}

bool SilencerPluginAction::ParseArgs(const CompilerInstance &,
                                     const std::vector<std::string> &) {
  return true;
}

std::unique_ptr<clang::ASTConsumer>
SilencerPluginAction::CreateASTConsumer(clang::CompilerInstance &CI,
                                        llvm::StringRef file) {
  Rewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
  return std::make_unique<SilencerASTConsumer>(Rewriter);
}

void SilencerPluginAction::EndSourceFileAction() {
#if 1
  /// Rewriter.getEditBuffer(Rewriter.getSourceMgr().getMainFileID())
  ///     .write(llvm::outs());
  Rewriter.overwriteChangedFiles();
#endif
}

//-----------------------------------------------------------------------------
// Registration
//-----------------------------------------------------------------------------
static FrontendPluginRegistry::Add<SilencerPluginAction>
    X(/*Name=*/"silencer",
      /*Desc=*/"Rewrites code so that there's no warning about unused local "
               "variables or function args");
