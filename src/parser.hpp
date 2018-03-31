#pragma once

#include <map>
using std::map;

#include <llvm/IR/BasicBlock.h>

#include "ast.hpp"
#include "lexer.hpp"


using CLIter = typename vector<Lexem>::const_iterator;
struct ParseResult {
    CLIter cursor;
    BaseAST* ast = nullptr;

    ParseResult() = default;
    ParseResult(CLIter result, BaseAST* tree=nullptr) :
        cursor(result), ast(tree) {}

    bool isEmpty() const { return ast == nullptr; }
};

class BaseParser {
public:
    BaseParser() {}
    virtual ParseResult parse(CLIter, CLIter) const = 0;
    virtual ~BaseParser() {}
};


class IntegerParser final : public BaseParser {
public:
    IntegerParser() : BaseParser() {}
    virtual ~IntegerParser() {}

    virtual ParseResult parse(CLIter, CLIter) const override final;
};


class NameParser final : public BaseParser {
    llvm::BasicBlock* block = nullptr;

public:
    NameParser(llvm::BasicBlock* b) : BaseParser(), block(b) {}
    ~NameParser() override {}

    ParseResult parse(CLIter, CLIter) const override final;
};


class OperandParser final : public BaseParser {
    /* OPERAND = NUMBER | VARIABLE | CALL_INSTR */

    llvm::BasicBlock* block = nullptr;
    vector<BaseParser*> alternatives;

public:
    OperandParser(llvm::BasicBlock* b);
    virtual ~OperandParser() override {
        for (BaseParser* alt: alternatives)
            delete alt;
    }

    virtual ParseResult parse(CLIter, CLIter) const override final;
};


class BinaryParser final : public BaseParser {
    /*
     * BINARY = OPERAND + OPERATOR + BINARY |
     *          '(' + OPERAND + OPERATOR + BINARY + ')' |
     *          OPERAND
    */
    llvm::BasicBlock* block = nullptr;

public:
    BinaryParser(llvm::BasicBlock* b) : BaseParser(), block(b) {}
    virtual ~BinaryParser() override {}

    virtual ParseResult parse(CLIter, CLIter) const override final;

private:
    bool terminate(const Tag tag) const;
    ParseResult takeOperand(CLIter, CLIter) const;
    string takeOperator(const CLIter, const CLIter) const;
};


class CallInstrParser final : public BaseParser {
    llvm::BasicBlock* block;

public:
    CallInstrParser(llvm::BasicBlock* basicblock) :
        BaseParser(), block(basicblock) {}
    virtual ~CallInstrParser() {}

    virtual ParseResult parse(CLIter, CLIter) const override final;
};


class AssignInstrParser final : public BaseParser {
    /* ASSIGN_INSTR = NAME + '=' BINARY_INSTR */
    llvm::BasicBlock* block;

public:
    AssignInstrParser(llvm::BasicBlock* b);
    virtual ~AssignInstrParser() {}

    virtual ParseResult parse(CLIter, CLIter) const override final;
};
