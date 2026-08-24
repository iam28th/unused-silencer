#include "PluginActionShared.h"

#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>

using namespace llvm;
using namespace clang;

static llvm::cl::OptionCategory SilencerCategory("silencer options");

static cl::opt<bool> Inplace{"inplace", cl::desc("modify input inplace"),
                             cl::init(false), cl::cat(SilencerCategory)};

class SilencerPluginAction_Tool : public SilencerPluginActionShared {
public:
  bool ParseArgs(const clang::CompilerInstance &,
                 const std::vector<std::string> &) override {
    return true;
  }

  /// A hook that runs after TU was finished
  void EndSourceFileAction() override {
    if (Inplace) {
      Rewriter.overwriteChangedFiles();
      llvm::outs() << "Changes applied\n";
    }
  }
};

int main(int Argc, const char **Argv) {
  llvm::Expected<tooling::CommonOptionsParser> eOptParser =
      clang::tooling::CommonOptionsParser::create(Argc, Argv, SilencerCategory);
  if (auto E = eOptParser.takeError()) {
    errs() << "Problem constructing CommonOptionsParser "
           << toString(std::move(E)) << '\n';
    return EXIT_FAILURE;
  }
  clang::tooling::ClangTool Tool(eOptParser->getCompilations(),
                                 eOptParser->getSourcePathList());

  return Tool.run(
      clang::tooling::newFrontendActionFactory<SilencerPluginAction_Tool>()
          .get());
}
