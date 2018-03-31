#pragma once

#include <string>
using std::string;
#include <ostream>
using std::ostream;
#include <array>
using std::array;
#include <vector>
using std::vector;

#include "exceptions.hpp"


enum class Tag : unsigned short int {
    SEPARATOR = 0,      //[ ]
    INTEGER = 1,        //\d+
    OPERATOR = 2,       //+, -, /, ...
    BLOCK = 4,          /* define add(a, b):
                        [    ]return a + b */
    EOL = 5,            //\n
    KEYWORD = 6,        //define, if, else, ...
    NAME = 7,           //\w+
    LEFT_BRACKET = 9,   //(
    RIGHT_BRACKET = 10, //)
    COMMA = 11,         //,
    COLON = 12,         //:
    ASSIGN = 13,        //=
    UNKNOWN = 256,
};

ostream& operator << (ostream& os, Tag tag);

class Lexem {
    bool empty = true;
    Tag tag = Tag::UNKNOWN;
    string content;
    unsigned int start;
    unsigned int end;
    unsigned int length;

public:
    Lexem() = default;
    explicit Lexem(const string& cont, unsigned int s, unsigned int e, Tag t);
    ~Lexem() = default;

    string getContent() const { return tag == Tag::EOL ? "\\n" : content; }
    unsigned int getLength() const { return length; }
    Tag getTag() const { return tag; }

    bool isEmpty() const { return empty && tag == Tag::UNKNOWN; }
};
ostream& operator << (ostream& os, const Lexem& lexem);



class Token {
    Tag tag;
    string string_pattern;

public:
    Token() = default;
    explicit Token(const string& _template, const Tag& _tag);
    explicit Token(const char* _template, const Tag& _tag);
    ~Token() = default;

    Tag getTag() const { return tag; }
    string getPattern() const { return string_pattern; }

    Lexem match(const string& str, const unsigned int position=0) const;
};


/*
        ***TERMINALS***
    VOID =
    INTEGER = [0-9]+
    NAME = ([A-Z]|[a-z])+[0-9]
    OPERATOR = +|-|/|*
    ASSIGN = `=`
    COMMA = `,`
    SEPARATOR = ` `
    EOL = `\n`
    BLOCK_TR = [ ]{4}
        ***INSTRUCTIONS***
    OPERAND = INTERGER | NAME | CALL_INSTR
    RHS = OPERATOR + BINARY_INSTR
    LHS = OPERAND

    COMMA_SEPARATED_RHS = COMMA + SEPARATOR? + COMMA_SEPARATED_LIST
    COMMA_SEPARATED_LHS = OPERAND
    COMMA_SEPARATED_LIST = COMMA_SEPARATED_LHS + COMMA_SEPARATED_RHS?

    BINARY_INSTR = LHS + RHS?
    CALL_INSTR = NAME + `(` + VOID | COMMA_SEPARATED_LIST + `)`
    ASSIGN_INSTR = NAME + `=` + (BINARY_INSTR | CALL_INSTR)
    RETURN_INSTR = `return` (VOID | BINARY_INSTR) + EOL
    INSTRUCTION = BLOCK_TR+ + (ASSIGN_INSTR | CALL_INSTR) + EOL
        ***FUNCTION***
    DECLARATION = `declare` F_NAME `(` PARAMETERS `)` + EOL
    F_NAME = NAME
    PARAMETERS = VOID | COMMA_SEPARATED_LIST
    PROTOTYPE = `define` + F_NAME + `(` + PARAMETERS + `)` + `:` + EOL
    BODY = INSTRUCTION* + RETURN_INSTR
    FUNCTION = PROTOTYPE + BODY
        ***MODULE***
    MAIN = FUNCTION(F_NAME="main", PARAMETERS=VOID)
    MODULE = (VOID | FUNCTION+) + MAIN
*/
class Lexer {
    const vector<Token> tokens = {
        Token("[ ]{4}", Tag::BLOCK),
        Token("[ ]", Tag::SEPARATOR),
        Token("\\d+", Tag::INTEGER),
        Token("\\n", Tag::EOL),
        Token("(define)|(while)|(if)|(else)|(return)|(declare)", Tag::KEYWORD),
        Token("\\w+", Tag::NAME),
        Token(",", Tag::COMMA),
        Token(":", Tag::COLON),
        Token("\\(", Tag::LEFT_BRACKET),
        Token("\\)", Tag::RIGHT_BRACKET),
        Token("(\\-)|(\\+)|(/)|(\\*)|(==)", Tag::OPERATOR),
        Token("=", Tag::ASSIGN),
    };

    unsigned int line_count;
    unsigned int row_count;

public:
    Lexer();

    vector<Lexem> tokenize(const string& str);
};
