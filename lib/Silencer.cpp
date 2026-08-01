#include "Silencer.h"

#include <clang/AST/Decl.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Frontend/FrontendPluginRegistry.h>
#include <llvm/Support/raw_ostream.h>

using namespace clang;
using namespace ast_matchers;

void ParamHandler::run(const clang::ast_matchers::MatchFinder::MatchResult &Result) {
  /// Print name and location of the argument
  ASTContext *Ctx = Result.Context;

  const FunctionDecl *Fn =
      Result.Nodes.getNodeAs<clang::FunctionDecl>("fn");
  assert(Fn && "nullptr in matcher callback");

  auto &SourceMgr = Rewriter.getSourceMgr();

  const int N = Fn->getNumParams();
  for (int i = 0; i < N; ++i) {
    const ParmVarDecl *Parm = Fn->getParamDecl(i);
    if (Parm->isUsed())
      continue;

    SourceLocation ParmLoc = Parm->getLocation();
    llvm::outs() << "ParmHandler: " << Parm->getDeclName() << " at " <<
      SourceMgr.getLineNumber(ParmLoc) << ":" << SourceMgr.getColumnNumber(ParmLoc) << " is unused!";
  }
}

void ParamHandler::onEndOfTranslationUnit() {}


void LocalVarHandler::run(const clang::ast_matchers::MatchFinder::MatchResult &) {}
void LocalVarHandler::onEndOfTranslationUnit() {}

SilencerASTConsumer::SilencerASTConsumer(clang::Rewriter &Rewriter) :
  ParamHandler(Rewriter), LocalVarHandler(Rewriter)
{
  DeclarationMatcher FunctionMatcher = functionDecl(
        isDefinition(),
        unless(isImplicit()),
        hasAnyParameter(anything())
    ).bind("fn");
  Finder.addMatcher(FunctionMatcher, &ParamHandler);

  DeclarationMatcher LocVarMatcher = varDecl(
      unless(parmVarDecl()),
      isDefinition(),
      hasAncestor(functionDecl(isDefinition()))
  ).bind("var");
  Finder.addMatcher(LocVarMatcher, &LocalVarHandler);
}

void SilencerASTConsumer::HandleTranslationUnit(clang::ASTContext &Ctx) {
  Finder.matchAST(Ctx);
}

class LACPluginAction : public PluginASTAction {
public:
  bool ParseArgs(const CompilerInstance &,
                 const std::vector<std::string> &) override {
    return true;
  }

  // Returns our ASTConsumer per translation unit.
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
    Rewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
    return std::make_unique<SilencerASTConsumer>(Rewriter);
  }

private:
  Rewriter Rewriter;
};

//-----------------------------------------------------------------------------
// Registration
//-----------------------------------------------------------------------------
static FrontendPluginRegistry::Add<LACPluginAction>
    X(/*Name=*/"silencer",
      /*Desc=*/"Rewrites code so that there's no warning about unused local variables or function args");
