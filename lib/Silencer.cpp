#include "Silencer.h"

#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Frontend/FrontendPluginRegistry.h>
#include <clang/Lex/Lexer.h>
#include <llvm/Support/raw_ostream.h>

using namespace clang;
using namespace ast_matchers;

using MatchResult = clang::ast_matchers::MatchFinder::MatchResult;

constexpr static char VarBinding[] = "var";
constexpr static char ParentBinding[] = "parent";
constexpr static char IfBinding[] = "if";

void ParamHandler::run(const MatchResult &Result) {
  ASTContext *Ctx = Result.Context;

  const FunctionDecl *Fn = Result.Nodes.getNodeAs<clang::FunctionDecl>("fn");
  assert(Fn && "nullptr in matcher callback");

  const int N = Fn->getNumParams();
  for (int i = 0; i < N; ++i) {
    const ParmVarDecl *Parm = Fn->getParamDecl(i);
    if (Parm->isUsed())
      continue;

    SourceLocation ParmLoc = Parm->getLocation();

    // construct fixit...
    auto Name = Parm->getNameAsString();
    const std::string FixItString = (Twine("/*") + Name + ("*/")).str();

    FixItHint FixItHint = FixItHint::CreateReplacement(
        SourceRange(ParmLoc, ParmLoc.getLocWithOffset(Name.size())),
        FixItString);

    DiagnosticsEngine &DiagEngine = Ctx->getDiagnostics();
    unsigned DiagID = DiagEngine.getCustomDiagID(DiagnosticsEngine::Warning,
                                                 "unused argument");
    DiagEngine.Report(Parm->getLocation(), DiagID).AddFixItHint(FixItHint);
    Rewriter.ReplaceText(ParmLoc, Name.length(), FixItString);
  }
}

/// Handles case when creating a new scope is not required for insertion
///
/// \param inIf true if match is in the beginning of if statement (when we need
/// to end insertion with comma)
static void handleDeclarationWithoutNewScope(const MatchResult &Result,
                                             bool inIf,
                                             clang::Rewriter &Rewriter) {
  const VarDecl *Var = Result.Nodes.getNodeAs<clang::VarDecl>(VarBinding);
  assert(Var && "nullptr in matcher callback");
  if (Var->isReferenced())
    return;

  const DeclStmt *Parent =
      Result.Nodes.getNodeAs<clang::DeclStmt>(ParentBinding);
  assert(Parent && "parent can't be null");

  const std::string SuggestedFixString =
      (Twine(" (void)") + Var->getNameAsString() + (inIf ? "," : ";")).str();

  ASTContext *Ctx = Result.Context;
  const auto &SM = Ctx->getSourceManager();
  auto InsertLoc = Lexer::getLocForEndOfToken(Parent->getEndLoc(), 0, SM,
                                              Ctx->getLangOpts());
  FixItHint Hint = FixItHint::CreateInsertion(InsertLoc, SuggestedFixString);

  DiagnosticsEngine &DiagEngine = Ctx->getDiagnostics();
  unsigned DiagID =
      DiagEngine.getCustomDiagID(DiagnosticsEngine::Warning, "unused variable");

  DiagEngine.Report(Var->getLocation(), DiagID).AddFixItHint(Hint);

  Rewriter.InsertText(InsertLoc, SuggestedFixString);
}

/// Handles case when we need to insert a new scope around declaration
static void handleDeclarationWithNewScope(const MatchResult &Result,
                                          clang::Rewriter &Rewriter) {
  const VarDecl *Var = Result.Nodes.getNodeAs<clang::VarDecl>(VarBinding);
  assert(Var && "nullptr in matcher callback");
  if (Var->isReferenced())
    return;

  const DeclStmt *Parent =
      Result.Nodes.getNodeAs<clang::DeclStmt>(ParentBinding);
  assert(Parent && "parent can't be null");

  ASTContext *Ctx = Result.Context;
  const auto &SM = Ctx->getSourceManager();

  const std::string DeclString{SM.getCharacterData(Parent->getBeginLoc()),
                               SM.getCharacterData(Parent->getEndLoc())};

  const std::string SuggestedFixString =
      (Twine("{ ") + DeclString + "; (void)" + Var->getNameAsString() + "; }")
          .str();
  auto InsertLoc = Parent->getBeginLoc();

  FixItHint Hint = FixItHint::CreateInsertion(InsertLoc, SuggestedFixString);

  DiagnosticsEngine &DiagEngine = Ctx->getDiagnostics();
  unsigned DiagID =
      DiagEngine.getCustomDiagID(DiagnosticsEngine::Warning, "unused variable");

  DiagEngine.Report(Var->getLocation(), DiagID).AddFixItHint(Hint);

  auto ReplactedTextLength =
      DeclString.length() + 1; // + 1 for original semicolon
  Rewriter.ReplaceText(Parent->getBeginLoc(), ReplactedTextLength,
                       SuggestedFixString);
}

void CompoundStmtVarHandler::run(
    const clang::ast_matchers::MatchFinder::MatchResult &Result) {
  handleDeclarationWithoutNewScope(Result, /*inIf*/ false, Rewriter);
}

void IfStmtVarHandler::run(const MatchResult &Result) {
  const IfStmt *If = Result.Nodes.getNodeAs<clang::IfStmt>(IfBinding);
  const DeclStmt *Parent =
      Result.Nodes.getNodeAs<clang::DeclStmt>(ParentBinding);
  assert(If && "nullptr in matcher callback");
  assert(Parent && "nullptr in matcher callback");

  if (If->getInit() == llvm::dyn_cast<Stmt>(Parent))
    handleDeclarationWithoutNewScope(Result, /*inIf*/ true, Rewriter);
  else
    handleDeclarationWithNewScope(Result, Rewriter);
}

SilencerASTConsumer::SilencerASTConsumer(clang::Rewriter &Rewriter)
    : Rewriter(Rewriter), ParamHandler(Rewriter), IfStmtVarHandler(Rewriter),
      CompoundStmtVarHandler(Rewriter) {
  // clang-format off
  DeclarationMatcher FunctionMatcher =
    functionDecl(
      isDefinition(),
      unless(isImplicit()),
      hasAnyParameter(anything()))
   .bind("fn");

  DeclarationMatcher VarInIf =
    varDecl(
      hasParent(
        declStmt(hasParent(ifStmt().bind(IfBinding))
      ).bind(ParentBinding))
    ).bind(VarBinding);

  DeclarationMatcher VarInCompound =
    varDecl(
      unless(VarInIf),
      unless(parmVarDecl()),
      isDefinition(),
      hasAncestor(functionDecl(isDefinition())),
      hasParent(declStmt().bind(ParentBinding))
    ).bind(VarBinding);

  // clang-format on
  Finder.addMatcher(VarInIf, &IfStmtVarHandler);
  Finder.addMatcher(VarInCompound, &CompoundStmtVarHandler);
  Finder.addMatcher(FunctionMatcher, &ParamHandler);
}

void SilencerASTConsumer::HandleTranslationUnit(clang::ASTContext &Ctx) {
  Finder.matchAST(Ctx);
}

bool SilencerPluginAction_Plugin::ParseArgs(
    const CompilerInstance &, const std::vector<std::string> &args) {
  for (const auto &arg : args) {
    if (arg == "inplace") {
      ModifyInputInplace = true;
    }
  }
  return true;
}

void SilencerPluginAction_Plugin::EndSourceFileAction() {
  if (ModifyInputInplace) {
    Rewriter.overwriteChangedFiles();
  } else {
    Rewriter.getEditBuffer(Rewriter.getSourceMgr().getMainFileID())
        .write(llvm::outs());
  }
}

static FrontendPluginRegistry::Add<SilencerPluginAction_Plugin>
    X(/*Name=*/"silencer_plugin",
      /*Desc=*/"Rewrites code so that there's no warning about unused local "
               "variables or function args");
