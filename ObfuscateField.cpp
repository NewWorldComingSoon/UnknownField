#include "ObfuscateField.h"

std::map<std::string, std::vector<const clang::FieldDecl *>>
    GlobalFieldNodeVectorMap;
std::map<std::string, std::vector<std::string>>
    GlobalClassFieldDeclStringVectorMap;
std::map<std::string, bool> GlobalSDKUnknownFieldProtectionEnabledMap;

llvm::cl::OptionCategory
    UnknownFieldOptionCategory("UnknownField OptionCategory");

cl::opt<bool> GlobalObfucated{"g", cl::desc("Enable global obfucation"), cl::init(false),
                              cl::cat(UnknownFieldOptionCategory)};
