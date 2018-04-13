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
    if (ast == nullptr) return os << 0;
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
    auto* testSuite = new TestSuite;
    testSuite->addTest(new TestCaller<IntegerParser_Test>(
        "testInteger", &IntegerParser_Test::testParse
    ));
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

BinaryParser_Test::~BinaryParser_Test() {
    for (const pair<string, BaseAST*>& test: data)
        delete test.second;
}


void BinaryParser_Test::test() {
    for (const pair<string, BaseAST*>& test_data: data) {
        auto lexems = lexer.tokenize(test_data.first);
        ParseResult actual = parser.parse(lexems.begin(), lexems.end());
        if (*(actual.ast) == *(test_data.second)) {
            ASSERT(true);
        } else {
            ASSERT(false);
            cout << actual.ast << endl;
        }
        delete actual.ast;
    }
}

TestSuite* BinaryParser_Test::suite() {
    auto* suite = new TestSuite;
    suite->addTest(new TestCaller<BinaryParser_Test>(
        "test", &BinaryParser_Test::test)
    );
    return suite;
}


CallInstrParser_Test::CallInstrParser_Test() : TestCase()
    , block(llvm::BasicBlock::Create(context, "TestBlock"))
    , parser(CallInstrParser(block)) {}

CallInstrParser_Test::~CallInstrParser_Test() {
    for (const Pair& data: test_data) {
        delete data.second;
    }
}

void CallInstrParser_Test::test() {
    for (const Pair& data: test_data) {
        Lexems lexems = lexer.tokenize(data.first);
        ParseResult actual = parser.parse(lexems.begin(), lexems.end());
        if (*(actual.ast) == *(data.second)) {
            ASSERT(true);
        } else {
            cout << actual.ast << endl;
            cout << data.second << endl;
            ParseResult debug = parser.parse(lexems.begin(), lexems.end());
            delete debug.ast;
            ASSERT(false);
        }
        delete actual.ast;
    }
}

TestSuite* CallInstrParser_Test::suite() {
    auto* suite = new TestSuite;
    suite->addTest(new TestCaller<CallInstrParser_Test>(
        "test", &CallInstrParser_Test::test
    ));
    return suite;
}



AssignInstrParser_Test::AssignInstrParser_Test() : TestCase()
    , block(llvm::BasicBlock::Create(context, "testBlock"))
    , parser(AssignInstrParser(block)) {}
AssignInstrParser_Test::~AssignInstrParser_Test() {}


void AssignInstrParser_Test::test() {
    for (const Pair& data: testData) {
        Lexems lexems = lexer.tokenize(data.first);
        CLIter begin = lexems.begin();
        CLIter end = lexems.end();
        ParseResult actual = parser.parse(begin, end);
        if (*(actual.ast) == *(data.second)) {
            ASSERT(true);
        } else {
            auto debug = parser.parse(begin, end);
            cout << debug.ast << endl;
            cout << data.second << endl;
            delete debug.ast;
            ASSERT(false);
        }
        delete actual.ast;
    }
}

TestSuite* AssignInstrParser_Test::suite() {
    auto* suite = new TestSuite;
    suite->addTest(new TestCaller<AssignInstrParser_Test>(
        "test", &AssignInstrParser_Test::test
    ));
    return suite;
}
