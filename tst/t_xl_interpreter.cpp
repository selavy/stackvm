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
    Program program = {
        { xl::PUSH, 1.0 }
    };
    Interpreter interp(move(program));
    CPPUNIT_ASSERT(interp.run());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, interp.result(), TOLERANCE);
}

//------------------------------------------------------------------------------
CPPUNIT_TEST_SUITE_REGISTRATION(TestInterpreter);
