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
    MULT,
    DIV,

    _MAX_CODE
};
// verify that down casting to char will be okay
BOOST_STATIC_ASSERT(_MAX_CODE < std::numeric_limits<unsigned char>::max());

struct Instruction {
    unsigned char opcode;
    union {
        double operand;
        struct {
            int32_t funid;            
            int32_t nargs;
        } funparms;
    };

    Instruction() {}
    
    Instruction(OpCode code)
    : opcode(static_cast<unsigned char>(code))
    {}
    
    Instruction(OpCode code, double operand)
    : opcode(static_cast<unsigned char>(code))
    , operand(operand)
    {}

    Instruction(OpCode code, int32_t funid, int32_t nargs)
    : opcode(static_cast<unsigned char>(code))
    , operand(reinterpret_cast<double>((funid << sizeof(int32_t)) | (nargs)))
    {}
};

}
 
#endif // XL_INSTRUCTION__H_
