#ifndef _OBFUSCATE_FIELD_H
#define _OBFUSCATE_FIELD_H

#include <iostream>
#include <map>
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
#include "clang/Lex/MacroArgs.h"
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

#define UnknownFieldProtectionTagName "UnknownFieldProtection"

extern std::map<std::string, std::vector<const clang::FieldDecl *>>
    GlobalFieldNodeVectorMap;
extern std::map<std::string, std::vector<std::string>>
    GlobalClassFieldDeclStringVectorMap;
extern std::map<std::string, bool> GlobalSDKUnknownFieldProtectionEnabledMap;

extern llvm::cl::OptionCategory UnknownFieldOptionCategory;
extern cl::opt<bool> GlobalObfucated;

class ObfuscateFieldDeclHandler : public MatchFinder::MatchCallback {
public:
  ObfuscateFieldDeclHandler(Rewriter &Rewrite) : Rewrite(Rewrite) {}

  virtual void run(const MatchFinder::MatchResult &Result) {
    if (auto FDNode =
            Result.Nodes.getNodeAs<clang::FieldDecl>("ObfuscateField")) {
      auto FieldStr = Rewrite.getRewrittenText(FDNode->getSourceRange());
      if (FieldStr.empty()) {
        // Empty strings are not required
        return;
      }

      auto QualifiedNameStr = FDNode->getQualifiedNameAsString();
      auto ClassNameStr = QualifiedNameStr.substr(
          0, FDNode->getQualifiedNameAsString().length() -
                 FDNode->getNameAsString().length() - 2);

      if (FDNode->hasInClassInitializer()) {
        // Filter field that in class initializer
        GlobalSDKUnknownFieldProtectionEnabledMap[ClassNameStr] = false;
        errs() << "Error: Found field(" << FieldStr << ")"
               << " that in class(" << ClassNameStr << ")"
               << " initializer we don't support!\n ";
        return;
      }

      // Save sth that used later.
      GlobalFieldNodeVectorMap[ClassNameStr].push_back(FDNode);
      GlobalClassFieldDeclStringVectorMap[ClassNameStr].push_back(FieldStr);
    }
  }

private:
  Rewriter &Rewrite;
};

class ObfuscateFieldASTConsumer : public ASTConsumer {
public:
  ObfuscateFieldASTConsumer(Rewriter &R) : HandlerForObfuscateFieldDecl(R) {
    // Only match our main file.So we should use 'isExpansionInMainFile()'
    Matcher.addMatcher(
        traverse(TK_IgnoreUnlessSpelledInSource,
                 fieldDecl(isExpansionInMainFile()).bind("ObfuscateField")),
        &HandlerForObfuscateFieldDecl);
  }

  void HandleTranslationUnit(ASTContext &Context) override {
    Matcher.matchAST(Context);
  }

private:
  ObfuscateFieldDeclHandler HandlerForObfuscateFieldDecl;
  MatchFinder Matcher;
};

class PreprocessorPPCallback : public PPCallbacks {
public:
  /// Called by Preprocessor::HandleMacroExpandedIdentifier when a
  /// macro invocation is found.
  virtual void MacroExpands(const Token &MacroNameTok,
                            const MacroDefinition &MD, SourceRange Range,
                            const MacroArgs *Args) {

    if (GlobalObfucated) {
      // This is a global tag
      return;
    }

    auto IdentifierInfo = MacroNameTok.getIdentifierInfo();
    if (!IdentifierInfo) {
      return;
    }

    if (!Args) {
      return;
    }

    // We only care about our markers
    if (IdentifierInfo->getName().compare(UnknownFieldProtectionTagName) != 0) {
      return;
    }

    auto UnexpArgument0 = Args->getUnexpArgument(0);
    if (!UnexpArgument0) {
      return;
    }

    auto Arg0IdentifierInfo = UnexpArgument0->getIdentifierInfo();
    if (!Arg0IdentifierInfo) {
      return;
    }

    StringRef ClassName = Arg0IdentifierInfo->getName();
    GlobalSDKUnknownFieldProtectionEnabledMap[ClassName.str()] = true;
  }
};

class ObfuscateFieldFrontendAction : public ASTFrontendAction {
private:
  auto GenerateRandomKey() {
    std::random_device RD;
    // Generate seed
    std::mt19937 G(RD());
    return G;
  }

  bool EnableObfuscated(std::string ClassName) {
    if (GlobalObfucated) {
      // This is a global tag
      return true;
    }

    // Find class name that need to be protected.
    if (GlobalSDKUnknownFieldProtectionEnabledMap.find(ClassName) !=
        GlobalSDKUnknownFieldProtectionEnabledMap.end()) {
      if (GlobalSDKUnknownFieldProtectionEnabledMap[ClassName]) {
        return true;
      }
    }

    return false;
  }

public:
  ObfuscateFieldFrontendAction() {}
  void EndSourceFileAction() override {

    // For Field
    {
      // Traverse map
      for (auto &Map : GlobalClassFieldDeclStringVectorMap) {
        if (!EnableObfuscated(Map.first)) {
          continue;
        }

        // Shuffle it
        std::shuffle(Map.second.begin(), Map.second.end(), GenerateRandomKey());

        // Replace it
        size_t Count = Map.second.size();
        for (size_t i = 0; i < Count; ++i) {
          if (Map.second[i].empty()) {
            // Empty strings are not required
            continue;
          }
          auto FDNode = GlobalFieldNodeVectorMap[Map.first][i];
          TheRewriter.ReplaceText(FDNode->getSourceRange(),
                                  Map.second[i].c_str());
        }
      }
    }

#ifdef DEBUG_MODE
    TheRewriter.getEditBuffer(TheRewriter.getSourceMgr().getMainFileID())
        .write(llvm::outs());
#else
    TheRewriter.overwriteChangedFiles();
#endif
  }

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
    // Add PPCallbacks
    CI.getPreprocessor().addPPCallbacks(
        std::make_unique<PreprocessorPPCallback>());

    // New ASTConsumer
    TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
    return std::make_unique<ObfuscateFieldASTConsumer>(TheRewriter);
  }

private:
  Rewriter TheRewriter;
};

#endif
