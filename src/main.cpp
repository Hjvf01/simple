#include <iostream>
using std::endl;
using std::cout;
#include <cmath>

#ifdef RUN_TEST
    #include "../tests/test_parser/test_parser.h"
    #include "../tests/test_ast/test_ast.hpp"
#else
    #include "lexer.hpp"
#endif

int main() {
#ifdef RUN_TEST
    ParserTest::run();
    ASTTest::run();
#endif
    return 0;
}
