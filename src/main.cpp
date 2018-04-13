#include <iostream>
using std::endl;
using std::cout;

#ifdef RUN_TEST
    #include "../tests/test_parser/test_parser.h"
    #include "../tests/test_ast/test_ast.hpp"
#else
    #include "lexer.hpp"
    #include "ast.hpp"
    #include "parser.hpp"
#endif

int main() {
#ifdef RUN_TEST
    ASTTest::run();
    ParserTest::run();
#endif
    return 0;
}
