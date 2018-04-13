#include <cassert>

#include "parser.hpp"


ParseResult IntegerParser::parse(CLIter begin, CLIter end) const {
    ParseResult _default(begin);
    if (begin == end) return _default;

    string content = begin->getContent();
    return begin->getTag() == Tag::INTEGER ?
        ParseResult(begin + 1, new IntegerAST(std::atoi(content.c_str()))) :
        _default;
}




ParseResult NameParser::parse(CLIter begin, CLIter end) const {
    assert(block != nullptr);
    if (begin == end) return ParseResult(begin);

    return begin->getTag() == Tag::NAME ?
        ParseResult(begin + 1, new NameAST(begin->getContent(), block)) :
        ParseResult(begin);
}




OperandParser::OperandParser(llvm::BasicBlock *basicBlock) :
    BaseParser(), block(basicBlock) {
        alternatives = {
            new CallInstrParser(block),
            new NameParser(block),
            new IntegerParser
        };
}

ParseResult OperandParser::parse(CLIter begin, CLIter end) const {
    assert(block != nullptr);
    if (begin == end) return ParseResult(begin);

    for (BaseParser* alt: alternatives) {
        auto tempResult = alt->parse(begin, end);
        if (!tempResult.isEmpty()) {
            return tempResult;
        }
    }
    throw ParseError();
}




ParseResult BinaryParser::parse(CLIter begin, CLIter end) const {
    if (begin == end) return ParseResult(begin);

    ParseResult first = takeOperand(begin, end);
    CLIter current = first.cursor;
    BaseAST* root = first.ast;
    while (!finish(current, end)) {
        string op = takeOperator(current, end);
        current++;
        ParseResult second = takeOperand(current, end);
        current = second.cursor;
        root = buildTree(op, first.ast, second.ast);
        first.ast = root;
    }
    return ParseResult(current + (hasParen ? 1 : 0), root);
}

bool BinaryParser::finish(const CLIter cursor, const CLIter end) const {
    return (cursor == end) || (cursor->getTag() == Tag::RIGHT_BRACKET ||
        cursor->getTag() == Tag::EOL || cursor->getTag() == Tag::COMMA);
}

ParseResult BinaryParser::takeOperand(CLIter begin, CLIter end) const {
    switch (begin->getTag()) {
        case Tag::LEFT_BRACKET:
            return BinaryParser(block, true).parse(begin + 1, end);
        case Tag::NAME:
        case Tag::INTEGER:
            return OperandParser(block).parse(begin, end);
        default:
            throw ParseError();
    }
}

string BinaryParser::takeOperator(const CLIter begin, const CLIter end) const {
    assert(begin < end);

    if (begin->getTag() == Tag::OPERATOR)
        return begin->getContent();

    throw ParseError();
}

BinaryInstrAST* BinaryParser::buildTree(
    const string &op, BaseAST *lhs, BaseAST *rhs
) const {
    auto* leftSubTree = dynamic_cast<BinaryInstrAST*>(lhs);
    if (leftSubTree == nullptr)
        return new BinaryInstrAST(op, lhs, rhs, block, hasParen);

    auto* rightSubTree = dynamic_cast<BinaryInstrAST*>(rhs);
    if (rightSubTree == nullptr)
        return leftSubTree->weight() >= OP_WEIGHT.at(toOpCode(op)) ?
            new BinaryInstrAST(op, leftSubTree, rhs, block, hasParen) :
            new BinaryInstrAST(leftSubTree->getOperator(),
                leftSubTree->getLeft(), new BinaryInstrAST(op,
                    leftSubTree->getRight(), rhs, block
                ), block, hasParen
            );

    return new BinaryInstrAST(op, leftSubTree, rightSubTree, block, hasParen);
}




ArgumentParser::ArgumentParser(llvm::BasicBlock *b) : BaseParser(), block(b) {
    parsers = {
        new CallInstrParser(block),
        new BinaryParser(block),
        new NameParser(block),
        new IntegerParser
    };
}

ArgumentParser::~ArgumentParser() {
    for (BaseParser* parser: parsers)
        delete parser;
}

ParseResult ArgumentParser::parse(CLIter begin, CLIter end) const {
    for (BaseParser* parser: parsers) {
        ParseResult tempResult = parser->parse(begin, end);
        if (!tempResult.isEmpty())
            return tempResult;
    }
    throw ParseError();
}




ParseResult CallInstrParser::parse(CLIter begin, CLIter end) const {
    assert(begin < end);

    const string name = takeName(begin, end);
    CLIter current = begin + 1;
    if (name == "" || current->getTag() != Tag::LEFT_BRACKET)
        return ParseResult(begin);
    vector<BaseAST*> args;
    current += 1;
    while (!finish(current, end)) {
        ParseResult argument = takeArgument(current, end);
        args.push_back(argument.ast);
        current = skipComma(argument.cursor);
    }

    /*Skip close bracket and make AST*/
    return ParseResult(current + 1, new CallInstrAST(name, args, block));
}

bool CallInstrParser::finish(CLIter current, CLIter end) const {
    return current == end || current->getTag() == Tag::RIGHT_BRACKET;
}

CLIter CallInstrParser::skipComma(CLIter current) const {
    return current + (current->getTag() == Tag::COMMA ? 1 : 0);
}

string CallInstrParser::takeName(CLIter begin, CLIter end) const {
    assert(begin < end);

    if (begin->getTag() == Tag::NAME)
        return begin->getContent();
    return string();
}

ParseResult CallInstrParser::takeArgument(CLIter begin, CLIter end) const {
    return ArgumentParser(block).parse(begin, end);
}




AssignValueParser::AssignValueParser(llvm::BasicBlock *basicBlock) :
    BaseParser(), block(basicBlock) {
        parsers = {
            new CallInstrParser(block),
            new BinaryParser(block),
            new IntegerParser
        };
}

AssignValueParser::~AssignValueParser() {
    for (BaseParser* parser: parsers)
        delete parser;
}

ParseResult AssignValueParser::parse(CLIter begin, CLIter end) const {
    assert(begin < end);

    for (BaseParser* parser: parsers) {
        ParseResult tempResult = parser->parse(begin, end);
        if (!tempResult.isEmpty()) {
            return tempResult;
        }
    }
    throw ParseError();
}




AssignInstrParser::AssignInstrParser(llvm::BasicBlock *b) :
    BaseParser(), block(b) {}
AssignInstrParser::~AssignInstrParser() {}


ParseResult AssignInstrParser::parse(CLIter begin, CLIter end) const {
    if (begin == end) return ParseResult(begin);

    const string name = takeName(begin, end);
    CLIter current = skipAssignOperator(begin + 1);
    ParseResult value = takeValue(current, end);
    current = value.cursor;
    if (current->getTag() == Tag::EOL)
        return ParseResult(current, new AssignInstrAST(name, value.ast, block));
    throw ParseError();
}

string AssignInstrParser::takeName(const CLIter begin, const CLIter end) const {
    assert(begin < end);
    if (begin->getTag() == Tag::NAME)
        return begin->getContent();
    throw ParseError();
}

CLIter AssignInstrParser::skipAssignOperator(const CLIter current) const {
    if (current->getTag() == Tag::ASSIGN)
        return current + 1;
    throw ParseError();
}

using Res = ParseResult;
Res AssignInstrParser::takeValue(const CLIter begin, const CLIter end) const {
    assert(begin < end);
    return AssignValueParser(block).parse(begin, end);
}



