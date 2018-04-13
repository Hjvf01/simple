#include <cassert>
#include <regex>
using std::regex;
using std::match_results;
using std::sub_match;
#include <memory>
using std::allocator;
#include <algorithm>
using std::remove_if;

#include "lexer.hpp"


ostream& operator << (ostream& os, Tag tag) {
    switch (tag) {
        case Tag::SEPARATOR: return os << "Tag::SEPARATOR";
        case Tag::INTEGER: return os << "Tag::INTEGER";
        case Tag::OPERATOR: return os << "Tag::OPERATOR";
        case Tag::BLOCK: return os << "Tag::BLOCK";
        case Tag::EOL: return os << "Tag::EOL";
        case Tag::KEYWORD: return os << "Tag::KEYWORD";
        case Tag::NAME: return os << "Tag::NAME";
        case Tag::LEFT_BRACKET: return os << "Tag::LEFT_BRACKET";
        case Tag::RIGHT_BRACKET: return os << "Tag::RIGHT_BRACKET";
        case Tag::COMMA: return os << "Tag::COMMA";
        case Tag::COLON: return os << "Tag::COLON";
        case Tag::ASSIGN: return os << "Tag::ASSIGN";
        default: return os << "Tag::UNKNOWN";
    }
}


Lexem::Lexem(const std::string &cont, unsigned int s, unsigned int e, Tag t) :
    content(cont), start(s), end(e), length(e - s), tag(t) {
            assert(start < end);
            assert(length == content.size());
            empty = false;
}


ostream& operator << (ostream& os, const Lexem& lexem) {
    return os << "[Lexem: content - '" << lexem.getContent() << "' tag - " <<
        lexem.getTag() << "]";
}



Token::Token(const char *_template, const Tag &_tag) :
    string_pattern(_template), tag(_tag) {}
Token::Token(const string& _template, const Tag& _tag) :
        string_pattern(_template), tag(_tag) {}

Lexem Token::match(const std::string &str, const unsigned int position) const {
    using CIter = string::const_iterator;
    using CAllocator = allocator<sub_match<CIter>>;

    regex pattern(string_pattern);
    match_results<CIter, CAllocator> match_r;

    bool result = std::regex_search(
        str.begin() + position, str.end(), match_r, pattern
    );

    return result && match_r.position() == 0 ?
        Lexem(match_r.str(), position, position + match_r.length(), tag) :
        Lexem();
}


Lexer::Lexer() : line_count(0), row_count(0) {}

vector<Lexem> Lexer::tokenize(const string& str) const throw(SyntaxError) {
    const unsigned int length = str.size();
    unsigned int position = 0;
    vector<Lexem> lexems = {};

    if (length == 0) return lexems;

    while (position < length) {
        const Lexem lex = findLexem(str, position);
        position += lex.getLength();
        if (lex.getTag() != Tag::SEPARATOR)
            lexems.push_back(lex);
    }
    return lexems;
}

Lexem Lexer::findLexem(const string& str, const unsigned int position) const {
    for (const Token& token: tokens) {
        Lexem match_result = token.match(str, position);
        if (!match_result.isEmpty()) {
            return match_result;
        }
    }
    throw SyntaxError();
}
