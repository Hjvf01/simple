#pragma once

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
    BaseParser() = default;
    virtual ParseResult parse(CLIter, CLIter) const = 0;
    virtual ~BaseParser() = default;
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
    bool hasParen;

public:
    BinaryParser(llvm::BasicBlock* b, const bool paren=false) :
        BaseParser(), block(b), hasParen(paren) {}
    virtual ~BinaryParser() override {}

    virtual ParseResult parse(CLIter, CLIter) const override final;

private:
    ParseResult takeOperand(CLIter, CLIter) const;
    string takeOperator(const CLIter, const CLIter) const;
    BinaryInstrAST* buildTree(const string&, BaseAST*, BaseAST*) const;
    bool finish(const CLIter, const CLIter) const;
};


class ArgumentParser final : public BaseParser {
    /* ARG = NAME | INTEGER | CALL | BINARY */

    vector<BaseParser*> parsers;
    llvm::BasicBlock* block = nullptr;

public:
    ArgumentParser(llvm::BasicBlock* b);
    virtual ~ArgumentParser() override final;

    virtual ParseResult parse(CLIter, CLIter) const override final;
};


class CallInstrParser final : public BaseParser {
    /*
     * CALL = NAME + '(' + VOID | ARGS + ')' + EOL?
     * ARGS = ARG | ARG + ',' + ARGS
     * ARG = NAME | INTEGER | CALL | BINARY
     * VOID =
    */
    llvm::BasicBlock* block;

public:
    CallInstrParser(llvm::BasicBlock* basicblock) :
        BaseParser(), block(basicblock) {}
    virtual ~CallInstrParser() {}

    virtual ParseResult parse(CLIter, CLIter) const override final;

private:
    bool finish(CLIter, CLIter) const;
    string takeName(CLIter, CLIter) const;
    ParseResult takeArgument(CLIter, CLIter) const;
    CLIter skipComma(CLIter) const;
};


class AssignValueParser final : public BaseParser {
    /* VALUE = INTEGER | CALL | BINARY */

    vector<BaseParser*> parsers;
    llvm::BasicBlock* block = nullptr;

public:
    AssignValueParser(llvm::BasicBlock* basicBlock);
    virtual ~AssignValueParser() override final;

    virtual ParseResult parse(CLIter, CLIter) const override final;
};


class AssignInstrParser final : public BaseParser {
    /*
     * ASSIGN = TYPE_NAME + NAME + '=' VALUE
     * VALUE = INTEGER | CALL | BINARY
     */

    llvm::BasicBlock* block;

public:
    AssignInstrParser(llvm::BasicBlock* b);
    virtual ~AssignInstrParser();

    virtual ParseResult parse(CLIter, CLIter) const override final;

private:
    string takeName(const CLIter, const CLIter) const;
    CLIter skipAssignOperator(const CLIter) const;
    ParseResult takeValue(const CLIter, const CLIter) const;
};
