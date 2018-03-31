#include <cassert>
#include <algorithm>
using std::distance;

#include "parser.hpp"


ParseResult IntegerParser::parse(CLIter begin, CLIter end) const {
    assert(begin != end);

    string content = begin->getContent();
    return begin->getTag() == Tag::INTEGER ?
        ParseResult(begin + 1, new IntegerAST(std::atoi(content.c_str()))) :
        ParseResult(begin, nullptr);
}


ParseResult NameParser::parse(CLIter begin, CLIter end) const {
    assert(begin != end);
    assert(block != nullptr);

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
    assert(begin != end);
    assert(block != nullptr);

    ParseResult result;
    for (BaseParser* alt: alternatives) {
        auto tempResult = alt->parse(begin, end);
        if (!tempResult.isEmpty()) {
            result = tempResult;
            break;
        }
    }
    return result;
}




ParseResult BinaryParser::parse(CLIter begin, CLIter end) const {
    assert(begin != end);
    assert(block != nullptr);

    CLIter next = begin + 1;
    bool terminate = next->getTag() == Tag::EOL ||
        next->getTag() == Tag::RIGHT_BRACKET ||
        next->getTag() == Tag::COMMA;
    if (next == end || terminate)
        return OperandParser(block).parse(begin, end);

    if (distance(begin, end) == 2)
        throw ParseError();

    ParseResult lhs = takeOperand(begin, end);
    string oper = takeOperator(lhs.cursor + 1, end);
    ParseResult rhs = parse(lhs.cursor + 2, end);
    BinaryInstrAST* rhsAST = dynamic_cast<BinaryInstrAST*>(rhs.ast);

    if (rhsAST == nullptr) {
        return ParseResult(rhs.cursor, new BinaryInstrAST(
            oper, lhs.ast, rhs.ast, block
        ));
    }
    const OpCode current = toOpCode(oper);
    if (rhsAST->weight() >= OP_WEIGHT.at(current)) {
        return ParseResult(rhs.cursor, new BinaryInstrAST(
            oper, lhs.ast, rhs.ast, block
        ));
    } else {
        return ParseResult(rhs.cursor, new BinaryInstrAST(
            rhsAST->getOperator(), new BinaryInstrAST(
                oper, lhs.ast, rhsAST->getLeft(), block
            ), rhsAST->getRight(), block
        ));
    }
}

bool BinaryParser::terminate(const Tag tag) const {
    return tag == Tag::EOL || tag == Tag::COMMA;
}

ParseResult BinaryParser::takeOperand(CLIter begin, CLIter end) const {
    return ParseResult(begin);
}

string BinaryParser::takeOperator(const CLIter begin, const CLIter end) const {
    assert(begin < end);

    if (begin->getTag() == Tag::OPERATOR)
        return begin->getContent();
    throw ParseError();
}


ParseResult CallInstrParser::parse(CLIter, CLIter) const {
    return ParseResult();
}
