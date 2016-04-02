#ifndef XL_INSTRUCTION__H_
#define XL_INSTRUCTION__H_

#include <boost/static_assert.hpp>
#include <limits>
#include <cstdint>

namespace xl {

enum OpCode {
    NOP=0,
    PUSH,
    POP,
    EQ,
    NEQ,
    PLUS,
    MINUS,
    MULT,
    DIV,
    CALL,

    _MAX_CODE
};
// verify that down casting to char will be okay
BOOST_STATIC_ASSERT(_MAX_CODE < std::numeric_limits<unsigned char>::max());

struct Instruction {
    unsigned char opcode;
    union {
        double operand;
        struct {
            uint32_t funid;            
            uint32_t nargs;
        } funparms;
    };
    Instruction();
    Instruction(OpCode code);
    Instruction(OpCode code, double operand);
    Instruction(OpCode code, uint32_t funid, uint32_t nargs);
};


const char* to_string(Instruction const& instr);

}
 
#endif // XL_INSTRUCTION__H_
