
#include "obfuscate_field.h"

static llvm::cl::OptionCategory
    UnknownFieldOptionCategory("UnknownField OptionCategory");

int main(int argc, const char **argv) {
  Expected<tooling::CommonOptionsParser> eOptParser =
      clang::tooling::CommonOptionsParser::create(
          argc, argv, UnknownFieldOptionCategory, llvm::cl::OneOrMore, nullptr,
          false);
  if (auto E = eOptParser.takeError()) {
    errs() << "Problem constructing CommonOptionsParser "
           << toString(std::move(E)) << '\n';
    return EXIT_FAILURE;
  }

  clang::tooling::ClangTool Tool(eOptParser->getCompilations(),
                                 eOptParser->getSourcePathList());
  return Tool.run(
      clang::tooling::newFrontendActionFactory<ObfuscateFieldFrontendAction>()
          .get());
}
