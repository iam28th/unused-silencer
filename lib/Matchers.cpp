#include "Matchers.h"

using namespace clang::ast_matchers;

// clang-format off
DeclarationMatcher FunctionMatcher =
    functionDecl(
      isDefinition(),
      unless(isImplicit()),
      hasAnyParameter(anything()))
   .bind(FunctionBinding);
// clang-format on
