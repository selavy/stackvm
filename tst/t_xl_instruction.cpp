#include "t_xl_instruction.h"
#include "xl_instruction.h"

void TestInstruction::callOpsPutInUnionCorrectly() {
    const uint32_t funid = 1;
    const uint32_t nargs = 5;
    xl::Instruction instr(xl::CALL, funid, nargs);
    CPPUNIT_ASSERT_EQUAL(funid, instr.funparms.funid);
    CPPUNIT_ASSERT_EQUAL(nargs, instr.funparms.nargs);
}

void TestInstruction::allInstructionsCanBePrinted() {
    for (int code = 0; code < xl::_MAX_CODE; ++code) {
        const xl::Instruction instr(static_cast<xl::OpCode>(code));
        const char* str = xl::to_string(instr);
        CPPUNIT_ASSERT(str);
    }
}

//------------------------------------------------------------------------------
CPPUNIT_TEST_SUITE_REGISTRATION(TestInstruction);
