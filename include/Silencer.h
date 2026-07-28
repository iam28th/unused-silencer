#pragma once

#include <clang/AST/ASTConsumer.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Rewrite/Core/Rewriter.h>

#if 1
class ArgMatcher
    : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  ArgMatcher(clang::Rewriter &Rewriter) : Rewriter(Rewriter) {}

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

class LocalVarMatcher
    : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  LocalVarMatcher(clang::Rewriter &Rewriter) : Rewriter(Rewriter) {}

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

  ArgMatcher ArgMatcher;
  LocalVarMatcher LocalVarMatcher;
};
