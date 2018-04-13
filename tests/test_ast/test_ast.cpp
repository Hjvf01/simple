#include <llvm/IR/ValueSymbolTable.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Instructions.h>

#include <llvm/ADT/SmallVector.h>

#include <llvm/Support/raw_ostream.h>

#include "test_ast.hpp"
using namespace ASTTest;

using std::cout;
using std::endl;


bool operator == (const BaseAST& left, const BaseAST& right) {
    return left.str() == right.str();
}
bool operator != (const BaseAST& left, const BaseAST& right) {
    return !(left.str() == right.str());
}
std::ostream& operator << (std::ostream& os, const BaseAST& ast) {
    return os << ast.str();
}


void AST_Test::testIntegerAST() {
    IntegerAST ast(45);
    ASSERT(ast == IntegerAST(
        std::atoi(Lexer().tokenize("45")[0].getContent().c_str())
    ));
}

llvm::Function* AST_Test::makeLLVMFunction(
    llvm::Type* ret, const string &name, Args&& args
) {
    return llvm::Function::Create(
        llvm::FunctionType::get(ret, args, false),
        llvm::GlobalValue::ExternalLinkage, name, &testModule
    );
}

void AST_Test::testNameAST() {
    llvm::Function* testFunction = makeLLVMFunction(IntegerType, "testNameAST",
            vector<llvm::Type*>{IntegerType, IntegerType}
    );
    auto* testBlock = llvm::BasicBlock::Create(
        testContext, "test", testFunction
    );
    auto* allocate = new llvm::AllocaInst(IntegerType, 4, "name", testBlock);
    auto* store = new llvm::StoreInst(
        llvm::ConstantInt::get(IntegerType, 2345), allocate, testBlock
    );
    llvm::Value* load = NameAST("name", testBlock).codegen();
    if (load == nullptr)
        return;
    llvm::ReturnInst::Create(testContext, load, testBlock);
    testFunction->print(llvm::outs());
    cout << endl;
}

void AST_Test::testBinaryInstrAST() {
    llvm::Function* testFunction = makeLLVMFunction(IntegerType,
        "testBinaryInstrAST", Args{IntegerType, IntegerType, IntegerType});
    auto* testBlock = llvm::BasicBlock::Create(
        testContext, "entry", testFunction);
    llvm::ReturnInst::Create(
        testContext, llvm::ConstantInt::get(IntegerType, 34), testBlock);
    testFunction->print(llvm::outs());
    cout << endl;
}

void AST_Test::testCallInstrAST() {
    //ASSERT(false);
}

void AST_Test::testAssignInstrAST() {
    //ASSERT(false);
}


TestSuite* AST_Test::suite() {
    using Pair = Pair<AST_Test>;
    auto* suite = new TestSuite;
    vector<Pair> cases = {
        Pair("testIntegerAST", &AST_Test::testIntegerAST),
        Pair("testNameAST", &AST_Test::testNameAST),
        Pair("testBinaryAST", &AST_Test::testBinaryInstrAST),
        Pair("testCallIntrAST", &AST_Test::testCallInstrAST),
        Pair("testAssignInstrAST", &AST_Test::testAssignInstrAST),
    };
    for (const Pair& test: cases) {
        suite->addTest(new TestCaller<AST_Test>(test.first, test.second));
    }
    return suite;
}
