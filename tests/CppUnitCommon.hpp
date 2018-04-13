#pragma once

#include <iostream>
#include <map>
#include <string>

#include <TestCase.h>
using CppUnit::TestCase;
#include <TestCaller.h>
using CppUnit::TestCaller;
#include <ui/text/TestRunner.h>
using CppUnit::TextUi::TestRunner;
#include <TestResult.h>
using CppUnit::TestResult;
#include <TestSuite.h>
using CppUnit::TestSuite;
#include <TestResultCollector.h>
using CppUnit::TestResultCollector;
#include <TextTestProgressListener.h>
#include <Test.h>

#define ASSERT CPPUNIT_ASSERT

template<typename Type> using Pair = std::pair<std::string, void(Type::*)()>;


class Listner final : public CppUnit::TextTestProgressListener {
    virtual void startTest(CppUnit::Test* test) override final {
        std::cout << test->getName() << std::endl;
    }
};
