#include "machine.h"
#include "instruction.h"

#define STACKSZ 128

int machine_execute(const struct instruction_t* program, const int32_t len, int64_t* result) {
    int32_t ip = 0;
    int64_t stack[STACKSZ];
    int32_t sp = 0;
    while (ip < len) {
        const struct instruction_t* inst = &program[ip++];
        switch (inst->opcode) {
        case kNOP:
            break;
        case kPUSH:
            stack[sp++] = inst->operand;
            break;
        case kPOP:
            --sp;
            break;
        case kJNZ:
            if (stack[sp - 1] != 0) {
                ip = (int32_t) inst->operand;
                if (ip >= len)
                    goto fail;
            }
            --sp;
            break;
        case kADD:
            stack[sp - 2] += stack[sp - 1];
            --sp;
            break;
        case kSUB:
            stack[sp - 2] -= stack[sp - 1];
            --sp;
            break;
        case kMUL:
            stack[sp - 2] *= stack[sp - 1];
            --sp;
            break;
        case kDIV:
            if (stack[sp - 1] == 0)
                goto fail;
            stack[sp - 2] /= stack[sp - 1];
            --sp;
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
fail:
    return 1;
}
