#pragma once

#include "../../src/parser.hpp"
#include "../../src/lexer.hpp"

#include <CppUnitCommon.hpp>

namespace ParserTest {
class IntegerParser_Test : public TestCase {
    IntegerParser parser;


public:
    void testParse();

    static CppUnit::TestSuite* suite();
};

class OperandParser_Test : public TestCase {
    llvm::BasicBlock* block = nullptr;
    OperandParser parser;

public:
    OperandParser_Test();
    ~OperandParser_Test() { delete block; }

    void testParse();

    static CppUnit::TestSuite* suite();
};


class BinaryParser_Test : public TestCase {
    llvm::BasicBlock* block = nullptr;
    BinaryParser parser;

public:
    BinaryParser_Test();
    ~BinaryParser_Test() { delete block; }

    void testParseSingleOperand();
    void testParseBinary();
    void testParseSimpleParen();

    static CppUnit::TestSuite* suite();
};


void run() {
    cout << __PRETTY_FUNCTION__ << endl;
    TestRunner runner;
    runner.addTest(IntegerParser_Test::suite());
    runner.addTest(OperandParser_Test::suite());
    //runner.addTest(BinaryParser_Test::suite());
    runner.run();
}
}
