#pragma once

#include "PluginActionShared.h"

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

/// Can't use the same plugin action class for both clang plugin and
/// a standalone tool because of different way they get options
class SilencerPluginAction_Plugin : public SilencerPluginActionShared {
public:
  bool ParseArgs(const clang::CompilerInstance &,
                 const std::vector<std::string> &) override;

  /// A hook that runs after TU processing was finished
  void EndSourceFileAction() override;

private:
  bool ModifyInputInplace = false;
};
