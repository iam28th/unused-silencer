#pragma once

#include <clang/AST/ASTConsumer.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Rewrite/Core/Rewriter.h>

#if 0
class UnusedArgMatcher
    : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  UnusedArgMatcher(clang::Rewriter &Rewriter) : Rewriter(Rewriter) {}

  // Executes whenever the Matcher in SilencerASTConsumer matches.
  void run(const clang::ast_matchers::MatchFinder::MatchResult &) override;

  // I wonder what we do here, especially since we have mutliple callbacks...
  // my current guess is 'nothing', and do stuff in HandleTranslationUnit
  // instead
  //
  // Callback that's executed at the end of the translation unit
  void onEndOfTranslationUnit() override;

private:
  clang::Rewriter &Rewriter;
  llvm::SmallSet<clang::FullSourceLoc, 8> EditedLocations;
};

class UnusedLocalVarMatcher
    : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  UnusedLocalVarMatcher(clang::Rewriter &Rewriter) : Rewriter(Rewriter) {}

  void run(const clang::ast_matchers::MatchFinder::MatchResult &) override;
  void onEndOfTranslationUnit() override;

private:
  clang::Rewriter &Rewriter;
  llvm::SmallSet<clang::FullSourceLoc, 8> EditedLocations;
};

class SilencerASTConsumer : public clang::ASTConsumer {
public:
  SilencerASTConsumer(clang::Rewriter &R);
  void HandleTranslationUnit(clang::ASTContext &Ctx) override {
    Finder.matchAST(Ctx);
  }

private:
  clang::ast_matchers::MatchFinder Finder;

  UnusedArgMatcher ArgMatcher;
  UnusedLocalVarMatcher LocalVarMatcher;
};
#endif
