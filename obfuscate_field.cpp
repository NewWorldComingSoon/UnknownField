#include "obfuscate_field.h"

std::map<std::string, std::vector<const clang::FieldDecl *>>
    GlobalFieldNodeVectorMap;
std::map<std::string, std::vector<std::string>>
    GlobalClassFieldDeclStringVectorMap;
