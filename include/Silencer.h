#pragma once

#include <clang/AST/ASTConsumer.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Rewrite/Core/Rewriter.h>

class ParamHandler : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  ParamHandler(clang::Rewriter &Rewriter) : Rewriter(Rewriter) {}

  // Executes whenever the Matcher in SilencerASTConsumer matches.
  void run(const clang::ast_matchers::MatchFinder::MatchResult &) override;

private:
  clang::Rewriter &Rewriter;
};

class CompoundStmtVarHandler
    : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  CompoundStmtVarHandler(clang::Rewriter &Rewriter) : Rewriter(Rewriter) {}

  void run(const clang::ast_matchers::MatchFinder::MatchResult &) override;

private:
  clang::Rewriter &Rewriter;
};

class IfStmtVarHandler
    : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  IfStmtVarHandler(clang::Rewriter &Rewriter) : Rewriter(Rewriter) {}

  void run(const clang::ast_matchers::MatchFinder::MatchResult &) override;

private:
  clang::Rewriter &Rewriter;
};

class SilencerASTConsumer : public clang::ASTConsumer {
public:
  /// Initializers matchers & registers them with Finder
  SilencerASTConsumer(clang::Rewriter &R);
  void HandleTranslationUnit(clang::ASTContext &Ctx) override;

private:
  clang::ast_matchers::MatchFinder Finder;
  clang::Rewriter &Rewriter;

  ParamHandler ParamHandler;
  IfStmtVarHandler IfStmtVarHandler;
  CompoundStmtVarHandler CompoundStmtVarHandler;
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
  bool ModifyInputInplace = false;
};
