#include "PluginActionShared.h"
#include "Silencer.h"

std::unique_ptr<clang::ASTConsumer>
SilencerPluginActionShared::CreateASTConsumer(clang::CompilerInstance &CI,
                                              llvm::StringRef file) {
  Rewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
  return std::make_unique<SilencerASTConsumer>(Rewriter);
}
