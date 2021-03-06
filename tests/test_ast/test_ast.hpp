#pragma once

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include "../src/lexer.hpp"
#include "../src/ast.hpp"

#include <CppUnitCommon.hpp>
using std::endl;
using std::cout;


namespace ASTTest {

using Args = std::vector<llvm::Type*>;

class AST_Test final : public TestCase {
    llvm::LLVMContext testContext;
    llvm::Module testModule;
    Lexer lexer;

    llvm::Type* VoidType;
    llvm::Type* IntegerType;


public:
    AST_Test() : TestCase(), testModule("AST_TestModule", testContext) {
        VoidType = llvm::Type::getVoidTy(testContext);
        IntegerType = llvm::IntegerType::get(testContext, 32);
    }
    ~AST_Test() {}

    void testIntegerAST();
    void testNameAST();
    void testBinaryInstrAST();
    void testCallInstrAST();
    void testAssignInstrAST();

    static TestSuite* suite();

private:
    llvm::Function* makeLLVMFunction(
        llvm::Type* retType, const string& name,
        Args&& args=Args()
    );
};

template<typename Test> void run() {
    std::cout << __PRETTY_FUNCTION__ << endl;
    TestResult controller;
    TestResultCollector collector;
    controller.addListener(&collector);
    Listner listner;
    controller.addListener(&listner);
    TestRunner runner;
    runner.addTest(Test::suite());
    runner.run(controller);
}

void run() {
    run<AST_Test>();
/*
    TestRunner runner;
    runner.addTest(AST_Test::suite());
    runner.run();
*/
}
}
