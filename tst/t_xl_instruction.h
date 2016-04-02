#ifndef T_XL_INSTRUCTION__H_
#define T_XL_INSTRUCTION__H_

#include <cppunit/extensions/HelperMacros.h>

class TestInstruction : public CppUnit::TestFixture {
 public:
    void callOpsPutInUnionCorrectly();
    void allInstructionsCanBePrinted();
    
    CPPUNIT_TEST_SUITE(TestInstruction);
    CPPUNIT_TEST(callOpsPutInUnionCorrectly);
    CPPUNIT_TEST(allInstructionsCanBePrinted);
    CPPUNIT_TEST_SUITE_END();
};

#endif // T_XL_INSTRUCTION__H_
