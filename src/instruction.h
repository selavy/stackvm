#ifndef INSTRUCTION__H_
#define INSTRUCTION__H_

#include <stdint.h>

enum opcode_t {
    kNOP = 0,

    // unary operators
    kPUSH,
    kPOP,

    // binary operators
    kADD,
    kSUB,
    kMUL,
    kDIV,

    // halt
    kHALT
};

struct instruction_t {
    enum opcode_t opcode;
    int64_t operand;
};

#endif // INSTRUCTION__H_
