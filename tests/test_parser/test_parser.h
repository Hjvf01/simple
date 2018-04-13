#pragma once

#include <map>
using std::map;
using std::pair;

#include "../../src/parser.hpp"
#include "../../src/lexer.hpp"

#include <CppUnitCommon.hpp>

namespace ParserTest {
class IntegerParser_Test : public TestCase {
    IntegerParser parser;

public:
    void testParse();

    static CppUnit::TestSuite* suite();
};

class OperandParser_Test : public TestCase {
    llvm::BasicBlock* block = nullptr;
    OperandParser parser;

public:
    OperandParser_Test();
    ~OperandParser_Test() { delete block; }

    void testParse();

    static CppUnit::TestSuite* suite();
};


class BinaryParser_Test : public TestCase {
    llvm::BasicBlock* block = nullptr;
    BinaryParser parser;
    Lexer lexer;

    const map<string, BaseAST*> data {
        {"4", new IntegerAST(4)},
        {"4 + 66", new BinaryInstrAST("+",
                new IntegerAST(4), new IntegerAST(66)
            )
        },
        {"23-67", new BinaryInstrAST("-",
                new IntegerAST(23), new IntegerAST(67)
            )
        },
        {"56  * name", new BinaryInstrAST("*",
                new IntegerAST(56), new NameAST("name")
            )
        },
        {"(9 / 0)", new BinaryInstrAST("/",
                new IntegerAST(9), new IntegerAST(0)
            )
        },
        {"(90 - 34) / 4", new BinaryInstrAST("/",
                new BinaryInstrAST("-", new IntegerAST(90), new IntegerAST(34)),
                new IntegerAST(4)
            )
        },
        {"((a  + b)/2) * h", new BinaryInstrAST("*",
                new BinaryInstrAST("/",
                    new BinaryInstrAST("+", new NameAST("a"), new NameAST("b")),
                    new IntegerAST(2)
                ),
                new NameAST("h")
            )
        },
    };

public:
    BinaryParser_Test();
    ~BinaryParser_Test();

    void test();

    static CppUnit::TestSuite* suite();
};

using Pair = std::pair<string, BaseAST*>;
using Args = vector<BaseAST*>;
class CallInstrParser_Test : public TestCase {

    llvm::BasicBlock* block = nullptr;
    CallInstrParser parser;
    Lexer lexer;

    const vector<Pair> test_data = {
        {"voidCall()", new CallInstrAST("voidCall", Args{})},
        {"sin(x)", new CallInstrAST("sin", Args{new NameAST("x")})},
        {"add(10, 2)", new CallInstrAST("add",
                Args{new IntegerAST(10), new IntegerAST(2)}
            )
        },
        {"function(n, d, (45 - 2) * 4)", new CallInstrAST("function",
                Args{
                    new NameAST("n"), new NameAST("d"),
                    new BinaryInstrAST("*", new BinaryInstrAST("-",
                            new IntegerAST(45), new IntegerAST(2)
                        ), new IntegerAST(4)
                    )
                }
            )
        },
        {"call(nested(45 / 3), name, void())", new CallInstrAST("call",
            Args{new CallInstrAST("nested", Args{new BinaryInstrAST("/",
                            new IntegerAST(45), new IntegerAST(3)
                        )
                    }
                ), new NameAST("name"), new CallInstrAST("void", Args{})
            }
        )}
    };

public:
    CallInstrParser_Test();
    virtual ~CallInstrParser_Test() override;

    void test();

    static TestSuite* suite();
};


class AssignInstrParser_Test final : public TestCase {
    llvm::BasicBlock* block = nullptr;
    AssignInstrParser parser;
    Lexer lexer;

    const vector<Pair> testData = {
        {"variable = 56\n", new AssignInstrAST("variable", new IntegerAST(56))},
        {"call = callable()\n", new AssignInstrAST("call",
                new CallInstrAST("callable", Args{})
            )
        },
        {"res = 2 + add(45, variable)\n", new AssignInstrAST("res",
            new BinaryInstrAST("+", new IntegerAST(2),
                new CallInstrAST("add", Args{
                    new IntegerAST(45), new NameAST("variable")})
                )
        )}
    };

public:
    AssignInstrParser_Test();
    ~AssignInstrParser_Test();

    void test();

    static TestSuite* suite();
};


void run() {
    cout << __PRETTY_FUNCTION__ << endl;
    TestRunner runner;
    runner.addTest(IntegerParser_Test::suite());
    runner.addTest(OperandParser_Test::suite());
    runner.addTest(BinaryParser_Test::suite());
    runner.addTest(CallInstrParser_Test::suite());
    runner.addTest(AssignInstrParser_Test::suite());
    runner.run();
}
}
