#include <stdio.h>
#include <inttypes.h>
#include <assert.h>
#include <lightning.h>

enum {
    OP_NOP,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_PSH,
    OP_POP,
};
typedef uint8_t Opcode;

struct Instruction {
    Opcode op;
    double dval;
};
typedef struct Instruction Instruction;

#define EBADOPCODE      1
#define ESTACKUNDERFLOW 2
#define ESTACKOVERFLOW  3

int VM_execute(Instruction *prog, const Instruction *endp, double *stack, int stacklen, double *result) {
    const Instruction *pc = prog;
    const Instruction *const end_pc = endp;
    size_t sp = 0;
    while (pc < end_pc) {
        switch (pc->op) {
        case OP_NOP:
            break;
        case OP_ADD:
            assert(sp >= 2);
            stack[sp - 2] = stack[sp - 2] + stack[sp - 1];
            --sp;
            break;
        case OP_SUB:
            assert(sp >= 2);
            stack[sp - 2] = stack[sp - 2] - stack[sp - 1];
            --sp;
            break;
        case OP_MUL:
            assert(sp >= 2);
            stack[sp - 2] = stack[sp - 2] * stack[sp - 1];
            --sp;
            break;
        case OP_DIV:
            assert(sp >= 2);
            stack[sp - 2] = stack[sp - 2] / stack[sp - 1];
            --sp;
            break;
        case OP_PSH:
            stack[sp++] = pc->dval;
            break;
        case OP_POP:
            --sp;
            break;
        default:
            return -EBADOPCODE;
        }
        ++pc;
    }

    if (sp > 1) {
        return -ESTACKUNDERFLOW;
    } else if (sp == 0) {
        return -ESTACKOVERFLOW;
    }
    *result = stack[0];
    return 0;
}

void TEST_add() {
    const double A = 5.5;
    const double B = 6.;
    Instruction insts[] = {
        { .op=OP_PSH, .dval=A },
        { .op=OP_PSH, .dval=B  },
        { .op=OP_ADD, .dval=0.  }
    };
    const size_t len = sizeof(insts) / sizeof(*insts);
    const size_t stacklen = 10;
    double stack[stacklen];
    double result;
    int res = VM_execute(&insts[0],
                         &insts[len],
                         &stack[0],
                         stacklen,
                         &result);
    assert(res == 0);
    assert(result == (A + B));
}

void TEST_sub() {
    const double A = 5.5;
    const double B = 6.;
    Instruction insts[] = {
        { .op=OP_PSH, .dval=A },
        { .op=OP_PSH, .dval=B  },
        { .op=OP_SUB, .dval=0.  }
    };
    const size_t len = sizeof(insts) / sizeof(*insts);
    const size_t stacklen = 10;
    double stack[stacklen];
    double result;
    int res = VM_execute(&insts[0],
                         &insts[len],
                         &stack[0],
                         stacklen,
                         &result);
    assert(res == 0);
    assert(result == (A - B));
}

void TEST_mul() {
    const double A = 5.5;
    const double B = 6.;
    Instruction insts[] = {
        { .op=OP_PSH, .dval=A },
        { .op=OP_PSH, .dval=B  },
        { .op=OP_MUL, .dval=0.  }
    };
    const size_t len = sizeof(insts) / sizeof(*insts);
    const size_t stacklen = 10;
    double stack[stacklen];
    double result;
    int res = VM_execute(&insts[0],
                         &insts[len],
                         &stack[0],
                         stacklen,
                         &result);
    assert(res == 0);
    assert(result == (A * B));
}

void TEST_div() {
    const double A = 5.5;
    const double B = 6.;
    Instruction insts[] = {
        { .op=OP_PSH, .dval=A },
        { .op=OP_PSH, .dval=B  },
        { .op=OP_DIV, .dval=0.  }
    };
    const size_t len = sizeof(insts) / sizeof(*insts);
    const size_t stacklen = 10;
    double stack[stacklen];
    double result;
    int res = VM_execute(&insts[0],
                         &insts[len],
                         &stack[0],
                         stacklen,
                         &result);
    assert(res == 0);
    assert(result == (A / B));
}

int main(int argc, char **argv) {

    return 0;
}
