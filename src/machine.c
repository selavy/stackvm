#include "machine.h"
#include "instruction.h"

#define STACKSZ 128

int machine_execute(const struct instruction_t* program, const int32_t len, int64_t* result) {
    const struct instruction_t* ip = program;
    const struct instruction_t* end = ip + len;
    int64_t stack[STACKSZ];
    int32_t sp = 0;
    while (ip < end) {
        switch (ip->opcode) {
        case kNOP:
            ++ip;
            break;
        case kPUSH:
            stack[sp++] = ip->operand;
            ++ip;
            break;
        case kPOP:
            --sp;
            ++ip;
            break;
        case kADD:
            stack[sp - 2] += stack[sp - 1];
            --sp;
            ++ip;
            break;
        case kSUB:
            stack[sp - 2] -= stack[sp - 1];
            --sp;
            ++ip;
            break;
        case kHALT:
            goto halt;
            break;
        }
    }

halt:
    if (sp == 1) {
        *result = stack[0];
        return 0;
    } else {
        return 1;
    }
}
