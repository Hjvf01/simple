#include <string>
using std::string;
#include <iostream>
using std::endl;
using std::cout;
#include <map>

#include <TestSuite.h>
using CppUnit::TestSuite;

#include "test_parser.h"
using namespace ParserTest;

using Lexems = vector<Lexem>;

ostream& operator << (ostream& os, BaseAST* ast) {
    if (ast == nullptr)
        return os << 0;
    return os << ast->str();
}

ostream& operator << (ostream& os, const ParseResult& res) {
    return os << "[ParseResult: ast - " << res.ast << "]";
}
ostream& operator << (ostream& os, const BaseAST& ast) {
    return os << ast.str();
}

bool operator == (const BaseAST& left, const BaseAST& right) {
    return left.str() == right.str();
}
bool operator != (const BaseAST& left, const BaseAST& right) {
    return !(left == right);
}


void IntegerParser_Test::testParse() {
    string str = "345";
    Lexems lexems = Lexer().tokenize(str);
    ParseResult result = parser.parse(lexems.begin(), lexems.end());
    ASSERT(*(dynamic_cast<IntegerAST*>(result.ast)) == IntegerAST(345));

    str = "34 56 78";
    lexems = Lexer().tokenize(str);
    result = parser.parse(lexems.begin(), lexems.begin() + 1);
    ASSERT(*(dynamic_cast<IntegerAST*>(result.ast)) == IntegerAST(34));
    result = parser.parse(lexems.begin() + 1, lexems.begin() + 2);
    ASSERT(*(dynamic_cast<IntegerAST*>(result.ast)) == IntegerAST(56));
    ASSERT(lexems.begin() + 3 == lexems.end());
    result = parser.parse(lexems.begin() + 2, lexems.begin() + 3);
    ASSERT(*(dynamic_cast<IntegerAST*>(result.ast)) == IntegerAST(78));

    str = "sdf";
    lexems = Lexer().tokenize(str);
    result = parser.parse(lexems.begin(), lexems.end());
    ASSERT(result.isEmpty());
}


TestSuite* IntegerParser_Test::suite() {
    using Pair = Pair<IntegerParser_Test>;

    auto* testSuite = new TestSuite;
    vector<Pair> cases = {
        Pair("testParser", &IntegerParser_Test::testParse),
    };
    for(const Pair& _case: cases) {
        testSuite->addTest(new TestCaller<IntegerParser_Test>(
            _case.first, _case.second
        ));
    }
    return testSuite;
}

OperandParser_Test::OperandParser_Test() : TestCase()
    , block(llvm::BasicBlock::Create(context, "test_block"))
    , parser(OperandParser(block)) {}

void OperandParser_Test::testParse() {
    string str = "45";
    Lexems lexems = Lexer().tokenize(str);
    auto result = parser.parse(lexems.begin(), lexems.end());
    ASSERT(*(dynamic_cast<IntegerAST*>(result.ast)) == IntegerAST(45));

    str = "name";
    lexems = Lexer().tokenize(str);
    result = parser.parse(lexems.begin(), lexems.end());
    ASSERT(*(dynamic_cast<NameAST*>(result.ast)) == NameAST("name", block));
}

TestSuite* OperandParser_Test::suite() {
    auto* _suite = new TestSuite;
    _suite->addTest(new TestCaller<OperandParser_Test>(
        "testParse", &OperandParser_Test::testParse
    ));
    return _suite;
}



BinaryParser_Test::BinaryParser_Test() : TestCase()
    , block(llvm::BasicBlock::Create(context, "test_block"))
    , parser(BinaryParser(block)) {}

void BinaryParser_Test::testParseSingleOperand() {
    string str = "45";
    Lexems lexems = Lexer().tokenize(str);
    auto result = parser.parse(lexems.begin(), lexems.end());
    ASSERT(*(result.ast) == IntegerAST(45));
}

void BinaryParser_Test::testParseBinary() {
    string str = "34 + 34";
    Lexems lexems = Lexer().tokenize(str);
    auto result = parser.parse(lexems.begin(), lexems.end());
    ASSERT(*(result.ast) == BinaryInstrAST(
        "+", new IntegerAST(34), new IntegerAST(34), block
    ));

    str = "45 + 34 * 23";
    lexems = Lexer().tokenize(str);
    result = parser.parse(lexems.begin(), lexems.end());
    ASSERT(*(result.ast) == BinaryInstrAST("+", new BinaryInstrAST("*",
            new IntegerAST(34), new IntegerAST(23), block
        ), new IntegerAST(45), block
    ));

    str = "a + b / c";
    lexems = Lexer().tokenize(str);
    result = parser.parse(lexems.begin(), lexems.end());
    ASSERT(*(result.ast) == BinaryInstrAST("+", new BinaryInstrAST(
            "/", new NameAST("b", block), new NameAST("c", block), block
        ), new NameAST("a", block), block
    ));

    str = "45 + 2 * 90 - name";
    lexems = Lexer().tokenize(str);
    result = parser.parse(lexems.begin(), lexems.end());
    cout << result << endl;
}

void BinaryParser_Test::testParseSimpleParen() {}

TestSuite* BinaryParser_Test::suite() {
    using Pair = Pair<BinaryParser_Test>;
    auto* suite = new TestSuite;
    vector<Pair> cases = {
        Pair("testParseSingleOperand", &BinaryParser_Test::testParseSingleOperand),
        Pair("testParseBinary", &BinaryParser_Test::testParseBinary),
        Pair("testParseSimpleParen", &BinaryParser_Test::testParseSimpleParen)
    };
    for (const Pair& _case: cases) {
        suite->addTest(new TestCaller<BinaryParser_Test>(
            _case.first, _case.second
        ));
    }
    return suite;
}
