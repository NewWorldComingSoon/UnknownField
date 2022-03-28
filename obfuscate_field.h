#ifndef _OBFUSCATE_FIELD_H
#define _OBFUSCATE_FIELD_H

#include <iostream>
#include <random>
#include <set>
#include <string>
#include <vector>

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Refactoring.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;
using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::driver;
using namespace clang::tooling;

extern std::vector<const clang::FieldDecl *> GlobalFieldNodeVector;
extern std::vector<std::string> GlobalFieldDeclStringVector;

class ObfuscateFieldDeclHandler : public MatchFinder::MatchCallback {
public:
  ObfuscateFieldDeclHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &Result) {
    if (auto FDNode =
            Result.Nodes.getNodeAs<clang::FieldDecl>("ObfuscateField")) {
      auto FieldStr = Rewrite.getRewrittenText(FDNode->getSourceRange());
      GlobalFieldDeclStringVector.push_back(FieldStr);
      GlobalFieldNodeVector.push_back(FDNode);
    }
  }

private:
  Rewriter &Rewrite;
};

// Implementation of the ASTConsumer interface for reading an AST produced
// by the Clang parser. It registers a couple of matchers and runs them on
// the AST.
class ObfuscateFieldASTConsumer : public ASTConsumer {
public:
  ObfuscateFieldASTConsumer(Rewriter &R) : HandlerForObfuscateFieldDecl(R) {
    Matcher.addMatcher(traverse(TK_IgnoreUnlessSpelledInSource,
                                fieldDecl().bind("ObfuscateField")),
                       &HandlerForObfuscateFieldDecl);
  }

  void HandleTranslationUnit(ASTContext &Context) override {
    // Run the matchers when we have the whole TU parsed.
    Matcher.matchAST(Context);
  }

private:
  ObfuscateFieldDeclHandler HandlerForObfuscateFieldDecl;
  MatchFinder Matcher;
};

// For each source file provided to the tool, a new FrontendAction is created.
class ObfuscateFieldFrontendAction : public ASTFrontendAction {
private:
  auto GenerateRandomKey() {
    std::random_device rd;
    // Generate seed
    std::mt19937 g(rd());
    return g;
  }

public:
  ObfuscateFieldFrontendAction() {}
  void EndSourceFileAction() override {

    // For Field
    {
      // Shuffle it
      std::shuffle(GlobalFieldDeclStringVector.begin(),
                   GlobalFieldDeclStringVector.end(), GenerateRandomKey());
      // Replace it
      size_t Count = GlobalFieldNodeVector.size();
      for (size_t i = 0; i < Count; ++i) {
        auto FDNode = GlobalFieldNodeVector[i];
        TheRewriter.ReplaceText(FDNode->getSourceRange(),
                                GlobalFieldDeclStringVector[i].c_str());
      }
    }

    TheRewriter.getEditBuffer(TheRewriter.getSourceMgr().getMainFileID())
        .write(llvm::outs());
    // TheRewriter.overwriteChangedFiles();
  }

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
    TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
    return std::make_unique<ObfuscateFieldASTConsumer>(TheRewriter);
  }

private:
  Rewriter TheRewriter;
};

#endif
