#ifndef T_XL_INTERPRETER__H_
#define T_XL_INTERPRETER__H_

#include <cppunit/extensions/HelperMacros.h>

class TestInterpreter : public CppUnit::TestFixture {
 public:
    void emptyProgramInvalid();
    void pushAddsInstructionToStack();
    void nopDoesNothing();
    void popRemovesFromStack();
    void popEmptyStackNotValid();
    
    CPPUNIT_TEST_SUITE(TestInterpreter);
    CPPUNIT_TEST(emptyProgramInvalid);
    CPPUNIT_TEST(pushAddsInstructionToStack);
    CPPUNIT_TEST(nopDoesNothing);
    CPPUNIT_TEST(popRemovesFromStack);
    CPPUNIT_TEST(popEmptyStackNotValid);
    CPPUNIT_TEST_SUITE_END();
};

#endif // T_XL_INTERPRETER__H_
