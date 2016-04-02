#include "t_xl_interpreter.h"
#include "xl_instruction.h"
#include "xl_interpreter.h"

using namespace std;
using namespace xl;

static const double TOLERANCE = 0.00001;

void TestInterpreter::emptyProgramInvalid() {
    Program program;
    Interpreter interp(move(program));
    CPPUNIT_ASSERT(!interp.run());
}

void TestInterpreter::pushAddsInstructionToStack() {
    const double operand = 1.0;
    Program program = {
        { PUSH, operand }
    };
    Interpreter interp(move(program));
    CPPUNIT_ASSERT(interp.run());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, interp.result(), TOLERANCE);
}

void TestInterpreter::nopDoesNothing() {
    const double operand = 5.0;
    Program program = {
        { PUSH, operand },
        { NOP }
    };
    Interpreter interp(move(program));
    CPPUNIT_ASSERT(interp.run());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(operand, interp.result(), TOLERANCE);
}

void TestInterpreter::popRemovesFromStack() {
    const double operand = 5.0;
    Program program = {
        { PUSH, operand },
        { PUSH, 42.0    },
        { POP           }
    };
    Interpreter interp(move(program));
    CPPUNIT_ASSERT(interp.run());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(operand, interp.result(), TOLERANCE);
}

void TestInterpreter::popEmptyStackNotValid() {
    Program program = {
        { POP }
    };
    Interpreter interp(move(program));
    CPPUNIT_ASSERT(!interp.run());
}
                                               
void TestInterpreter::equalOperatorComparesTwoValues() {
    const double lhs = 3.0;
    const double rhs = 3.0;
    Program program = {
        { PUSH, lhs },
        { PUSH, rhs },
        { EQ }
    };
    Interpreter interp(move(program));
    CPPUNIT_ASSERT(interp.run());
    CPPUNIT_ASSERT_EQUAL(lhs == rhs, static_cast<bool>(interp.result()));
}

//------------------------------------------------------------------------------
CPPUNIT_TEST_SUITE_REGISTRATION(TestInterpreter);
