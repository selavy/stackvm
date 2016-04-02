#ifndef T_XL_INTERPRETER__H_
#define T_XL_INTERPRETER__H_

#include <cppunit/extensions/HelperMacros.h>

class TestInterpreter : public CppUnit::TestFixture {
 public:
    void emptyProgramInvalid();
    void pushAddsInstructionToStack();
    
    CPPUNIT_TEST_SUITE(TestInterpreter);
    CPPUNIT_TEST(emptyProgramInvalid);
    CPPUNIT_TEST(pushAddsInstructionToStack);
    CPPUNIT_TEST_SUITE_END();
};

#endif // T_XL_INTERPRETER__H_
