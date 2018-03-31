#include <exception>


class SyntaxError : public std::exception {
public:
    SyntaxError() = default;

    virtual const char* what() {
        return "Invalid token";
    }
};


class ParseError final : public std::exception {
public:
    ParseError() = default;

    virtual const char* what() {
        return "ParseError";
    }
};
