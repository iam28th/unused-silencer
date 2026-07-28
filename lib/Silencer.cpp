#if 1
#include "Silencer.h"

#include <clang/Frontend/FrontendPluginRegistry.h>

using namespace clang;
using namespace ast_matchers;

class LACPluginAction : public PluginASTAction {
public:
  // Our plugin can alter behavior based on the command line options
  bool ParseArgs(const CompilerInstance &,
                 const std::vector<std::string> &) override {
    return true;
  }

  // Returns our ASTConsumer per translation unit.
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
    return nullptr;
#if 0
    RewriterForLAC.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
    return std::make_unique<LACommenterASTConsumer>(RewriterForLAC);
#endif
  }

private:
  Rewriter RewriterForLAC;
};

//-----------------------------------------------------------------------------
// Registration
//-----------------------------------------------------------------------------
static FrontendPluginRegistry::Add<LACPluginAction>
    X(/*Name=*/"LAC",
      /*Desc=*/"Literal Argument Commenter");
#endif
