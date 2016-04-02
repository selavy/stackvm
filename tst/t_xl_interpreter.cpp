#include "t_xl_interpreter.h"
#include "xl_instruction.h"
#include "xl_interpreter.h"

using namespace std;
using namespace xl;

void TestInterpreter::emptyProgramInvalid() {
    Program program;
    Interpreter interp(move(program));
    CPPUNIT_ASSERT(!interp.run());
}

//------------------------------------------------------------------------------
CPPUNIT_TEST_SUITE_REGISTRATION(TestInterpreter);
