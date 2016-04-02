#include "xl_instruction.h"
#include <cassert>

namespace xl {

    Instruction::Instruction() {}

    Instruction::Instruction(OpCode code)
        : opcode(static_cast<unsigned char>(code))
    {}

    Instruction::Instruction(OpCode code, double operand)
        : opcode(static_cast<unsigned char>(code))
        , operand(operand)
    {}

    Instruction::Instruction(OpCode code, uint32_t funid, uint32_t nargs)
        : opcode(static_cast<unsigned char>(code))
#ifdef __GNUC__ // use gcc extension
        , funparms{funid, nargs}
    {}
#else
    {
        funparms.funid = funid;
        funparms.nargs = nargs;
    }
#endif

    const char* to_string(Instruction const& instr) {
        switch (instr.opcode) {
        case NOP:
            return "NOP";
        case PUSH:
            return "PUSH";
        case POP:
            return "POP";
        case EQ:
            return "EQ";
        case NEQ:
            return "NEQ";
        case PLUS:
            return "PLUS";
        case MINUS:
            return "MINUS";
        case MULT:
            return "MULT";
        case DIV:
            return "DIV";
        case CALL:
            return "CALL";
        }
        // show never get here
        assert(0);
        return 0;
    }
    
}
