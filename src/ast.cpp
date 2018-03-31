#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/ValueSymbolTable.h>

#include "ast.hpp"


OpCode toOpCode(const std::string &op) {
    OpCode code = OpCode::UNKNOWN;

    if (op == "-") code = OpCode::SUB;
    if (op == "+") code = OpCode::ADD;
    if (op == "*") code = OpCode::MUL;
    if (op == "/") code = OpCode::DIV;

    return code;
}

string toString(const OpCode code) {
    switch (code) {
        case OpCode::DIV:
            return "/";
        case OpCode::MUL:
            return "*";
        case OpCode::ADD:
            return "+";
        case OpCode::SUB:
            return "-";
        default:
            return "@";
    }
}


IntegerAST::IntegerAST(const int i) : BaseAST(), value(i) {}

llvm::Value* IntegerAST::codegen() {
    return llvm::ConstantInt::get(
        llvm::IntegerType::get(context, 32), value, true
    );
}

string IntegerAST::str() const {
    return "[IntegerAST: " + std::to_string(value) + "]";
}



NameAST::NameAST(const std::string &n, llvm::BasicBlock* b) :
    BaseAST(), name(n), block(b) {}

llvm::Value* NameAST::codegen() {
    llvm::Value* value = block->getValueSymbolTable()->lookup(name);
    return value != nullptr ?
        new llvm::LoadInst(value, name + ".load", block):
        nullptr;
}

string NameAST::str() const {
    return "[Name: " + name + "]";
}


llvm::Value* BinaryInstrAST::codegen() {
    llvm::Value* instruction = nullptr;
    llvm::Value* left = lhs->codegen();
    llvm::Value* right = rhs->codegen();
    return instruction;
}

string BinaryInstrAST::str() const {
    return "[BinaryInstrAST: " + toString(opCode) + " "
        + lhs->str() + " " + rhs->str() + "]";
}


llvm::Value* CallInstrAST::codegen() {
    llvm::Function* callFunction = module.getFunction(name);

    if (callFunction == nullptr) return nullptr;

    vector<llvm::Value*> argvs;
    for (BaseAST* arg: arguments)
        argvs.push_back(arg->codegen());

    return llvm::CallInst::Create(callFunction, argvs, name + ".call", block);
}

string CallInstrAST::str() const {
    string res = "[CallInstrAST call " + name + " (";

    const size_t length = arguments.size();
    if (length == 0) return res += ")]";

    for (size_t i = 0; i < length - 1; ++i)
        res += arguments[i]->str();
    return res += arguments[length - 1]->str() + ")]";
}



llvm::Value* AssignInstrAST::codegen() {
    return new llvm::StoreInst(value->codegen(), new llvm::AllocaInst(
            llvm::IntegerType::get(context, 32), 4, name, block
        ), block
    );
}

string AssignInstrAST::str() const {
    return "[AssignInstrAST: " + name + " = " + value->str() + "]";
}
