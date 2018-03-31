#pragma once

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include "../src/lexer.hpp"
#include "../src/ast.hpp"

#include <CppUnitCommon.hpp>
using std::endl;
using std::cout;

namespace ASTTest {
class AST_Test final : public TestCase {
    llvm::LLVMContext testContext;
    llvm::Module testModule;
    Lexer lexer;

public:
    AST_Test() : TestCase(), testModule("AST_TestModule", testContext) {}
    ~AST_Test() {}

    void testIntegerAST();
    void testNameAST();
    void testBinaryInstrAST();
    void testCallInstrAST();
    void testAssignInstrAST();

    static TestSuite* suite();

private:
    llvm::Function* makeVoidLLVMFunction(const string& name);
};

void run() {
    cout << __PRETTY_FUNCTION__ << endl;
    TestRunner runner;
    runner.addTest(AST_Test::suite());
    runner.run();
}
}
