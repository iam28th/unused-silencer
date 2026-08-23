#pragma once

#include <clang/AST/ASTConsumer.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Rewrite/Core/Rewriter.h>

/// Shared part between plugin and tool actions
class SilencerPluginActionShared : public clang::PluginASTAction
{
public:
  /// Returns our ASTConsumer per translation unit.
  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef file) override;

protected:
  clang::Rewriter Rewriter;
};
