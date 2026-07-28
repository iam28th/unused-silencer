#include "Silencer.h"

#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Frontend/FrontendPluginRegistry.h>
#include <llvm/Support/raw_ostream.h>

using namespace clang;
using namespace ast_matchers;

void ArgMatcher::run(const clang::ast_matchers::MatchFinder::MatchResult &Result) {
  /// Print name and location of the argument
  ASTContext *Ctx = Result.Context;

  const ParmVarDecl *Param =
      Result.Nodes.getNodeAs<clang::ParmVarDecl>("param");
  assert(Param && "nullptr in matcher callback");

  auto &SourceMgr = Rewriter.getSourceMgr();
  SourceLocation Sloc = Param->getLocation();

  llvm::outs() << "ArgMatcher: " << Param->getDeclName() << " at " <<
    SourceMgr.getLineNumber(Sloc) << ":" << SourceMgr.getColumnNumber(Sloc) << '\n';
}

void ArgMatcher::onEndOfTranslationUnit() {}


void LocalVarMatcher::run(const clang::ast_matchers::MatchFinder::MatchResult &) {}
void LocalVarMatcher::onEndOfTranslationUnit() {}

SilencerASTConsumer::SilencerASTConsumer(clang::Rewriter &Rewriter) :
  ArgMatcher(Rewriter), LocalVarMatcher(Rewriter)
{
  DeclarationMatcher ParamMatcher = parmVarDecl().bind("param");
  // use hasAncestor for now and wait for reply at
  // https://discourse.llvm.org/t/hasparent-ast-matcher-for-parmvardecl/91412
  //
  // just print for now, but later I guess...
  // 1. ignore explicit object parameters
  Finder.addMatcher(ParamMatcher, &ArgMatcher);
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
