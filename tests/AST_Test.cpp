#define CATCH_CONFIG_MAIN

#include <iostream>
using namespace std;

#include <catch.hpp>

#include "../src/ast.hpp"
#include "../src/lexer.hpp"


bool operator == (const BaseAST& left, const BaseAST& right) {
    return left.str() == right.str();
}
bool operator != (const BaseAST& left, const BaseAST& right) {
    return !(left.str() == right.str());
}

static Lexer lexer;


TEST_CASE("Test IntegerAST") {
    auto lexem = lexer.tokenize("45")[0];
    REQUIRE(IntegerAST(std::atoi(lexem.getContent().c_str())) == IntegerAST(45));
}
