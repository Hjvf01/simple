#pragma once

#include <string>
using std::string;
#include <vector>
using std::vector;
#include <map>
using std::map;

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/BasicBlock.h>
using Block = llvm::BasicBlock;


static llvm::LLVMContext context;
static llvm::Module module("unnamed", context);


enum class OpCode : unsigned short int {
    ADD = 0,
    SUB = 1,
    MUL = 2,
    DIV = 3,
    UNKNOWN = 255
};

static const map <OpCode, unsigned short int> OP_WEIGHT = {
    {OpCode::UNKNOWN, 0},
    {OpCode::ADD, 1},
    {OpCode::SUB, 1},
    {OpCode::MUL, 2},
    {OpCode::DIV, 2},
};

OpCode toOpCode(const string& op);
string toString(const OpCode code);


class Type {
public:
    Type() = default;
    virtual ~Type() = default;

    virtual string str() const = 0;
};


class IntegerType final : public Type {
    llvm::IntegerType* type;

public:
    IntegerType() : Type(), type(llvm::IntegerType::get(context, 32)) {}
    virtual ~IntegerType() override final {}

    string str() const override final;
};


class BaseAST {
public:
    BaseAST() = default;

    virtual llvm::Value* codegen() = 0;
    virtual string str() const = 0;
    virtual ~BaseAST() = default;
};


class IntegerAST final : public BaseAST {
    int value;

public:
    IntegerAST(const int i);
    ~IntegerAST() override {}

    llvm::Value* codegen() override;
    string str() const override;
};


class NameAST final : public BaseAST {
    string name;
    Block* block;
    Type* type;

public:
    NameAST(const string& n, Type* t) : BaseAST(),
        name(n), type(t),
        block(llvm::BasicBlock::Create(context, "default")) {}
    NameAST(const string& n, Block* b, Type* t);
    ~NameAST() override {}

    llvm::Value* codegen() override;
    string str() const override;
};


class BinaryInstrAST final : public BaseAST {
    BaseAST* lhs = nullptr;
    BaseAST* rhs = nullptr;
    Block* block = nullptr;
    OpCode opCode = OpCode::UNKNOWN;
    bool hasParen = false;

public:
    BinaryInstrAST(const string& op, BaseAST* l, BaseAST* r) :
        BaseAST(), opCode(toOpCode(op)), lhs(l), rhs(r)
        , block(llvm::BasicBlock::Create(context, "Default")),
        hasParen(false) {}
    BinaryInstrAST(
        const string& op, BaseAST* l, BaseAST* r, Block* b, bool paren=false) :
     BaseAST(), opCode(toOpCode(op)), lhs(l), rhs(r), block(b), hasParen(paren)
     {}

    ~BinaryInstrAST() override {
        delete lhs;
        delete rhs;
    }

    string getOperator() const { return toString(opCode); }
    BaseAST* getLeft() const { return lhs; }
    BaseAST* getRight() const { return rhs; }

    unsigned int weight() const {
        return OP_WEIGHT.at(opCode) + (hasParen ? 10 : 0);
    }

    llvm::Value* codegen() override;
    string str() const override;
};


class CallInstrAST final : public BaseAST {
    using Args = vector<BaseAST*>;

    string name;
    Args arguments;
    Block* block = nullptr;

public:
    CallInstrAST(const string& fName, const Args& args) : BaseAST()
        , name(fName), arguments(args)
        , block(Block::Create(context, "Default")) {}
    CallInstrAST(const string& fName, const Args& args, Block* b) :
        BaseAST(), name(fName), arguments(args), block(b) {}

    ~CallInstrAST() override {
        for (BaseAST* arg: arguments)
            delete arg;
    }

    llvm::Value* codegen() override;
    string str() const override;
};


class AssignInstrAST final : public BaseAST {
    string name;
    BaseAST* value = nullptr;
    Block* block = nullptr;

public:
    AssignInstrAST(const string& n, BaseAST* v) :
        BaseAST(), name(n), value(v),
        block(llvm::BasicBlock::Create(context, "Default")) {}
    AssignInstrAST(const string& n, BaseAST* v, Block* b) :
        BaseAST(), name(n), value(v), block(b) {}

    ~AssignInstrAST() override { delete value; }

    llvm::Value* codegen() override;
    string str() const override;
};


/* define function(Type: a, Type: b) -> Type:\n */
class PrototypeAST final : public BaseAST {
    string name;
    vector<string> arguments;
    llvm::Module* module;

public:
    PrototypeAST(const string&, const vector<string>&, llvm::Module*);
    virtual ~PrototypeAST() override final;

    llvm::Value* codegen() override;
    string str() const override;
};
