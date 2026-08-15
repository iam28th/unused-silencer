#pragma once

#include <clang/AST/ASTConsumer.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Rewrite/Core/Rewriter.h>

#if 1
class ParamHandler : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  ParamHandler(clang::Rewriter &Rewriter) : Rewriter(Rewriter) {}

  // Executes whenever the Matcher in SilencerASTConsumer matches.
  void run(const clang::ast_matchers::MatchFinder::MatchResult &) override;

  // Callback that's executed at the end of the translation unit
  //
  // ...I wonder what we do here, especially since we have mutliple callbacks...
  // my current guess is 'nothing', and do stuff in HandleTranslationUnit
  // instead
  void onEndOfTranslationUnit() override;

private:
  clang::Rewriter &Rewriter;
  llvm::SmallSet<clang::FullSourceLoc, 8> EditedLocations;
};

class LocalVarHandler : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  LocalVarHandler(clang::Rewriter &Rewriter) : Rewriter(Rewriter) {}

  void run(const clang::ast_matchers::MatchFinder::MatchResult &) override;
  void onEndOfTranslationUnit() override;

private:
  clang::Rewriter &Rewriter;
  llvm::SmallSet<clang::FullSourceLoc, 8> EditedLocations;
};

#endif

class SilencerASTConsumer : public clang::ASTConsumer {
public:
  /// Initializers matchers & registers them with Finder
  SilencerASTConsumer(clang::Rewriter &R);
  void HandleTranslationUnit(clang::ASTContext &Ctx) override;

private:
  clang::ast_matchers::MatchFinder Finder;
  clang::Rewriter &Rewriter;

  ParamHandler ParamHandler;
  LocalVarHandler LocalVarHandler;
};

class SilencerPluginAction : public clang::PluginASTAction {
public:
  bool ParseArgs(const clang::CompilerInstance &,
                 const std::vector<std::string> &) override;

  // Returns our ASTConsumer per translation unit.
  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef file) override;

  /// A hook that runs after TU was finished
  void EndSourceFileAction() override;

private:
  clang::Rewriter Rewriter;
};
