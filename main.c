#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_STACK 128
#define MAX_PROGRAM 128

#ifdef NDEBUG
#else
#define DEBUG
#endif

enum {
    OP_NOP = 0,
    OP_PUSH,
    OP_POP,
    OP_ADD,
    OP_SUB,
    OP_MULT,
    OP_DIV,
    OP_HALT,

    // must be last, also serves as count
    OP_INVALID,
};
typedef uint8_t Opcode;

const char *const _op_to_string[OP_INVALID+1] = {
    "NOP",
    "PUSH",
    "POP",
    "ADD",
    "SUB",
    "MULT",
    "DIV",
    "HALT",
    "INVALID",
};

/*static*/ const char *opcode_to_string(Opcode op) {
    op = op <= OP_INVALID ? op : OP_INVALID;
    return _op_to_string[op];
}

struct Instruction {
    Opcode opcode;          // 1 bytes
    union {
        double   dval;
        //uint64_t ival;
        struct {
            uint32_t func;
            uint32_t nargs;
        };
    } value;                // 8 bytes
};
typedef struct Instruction Instruction;

#define DVAL(i) (i).value.dval
#define FUNC(i) (i).value.func
#define NARGS(i) (i).value.nargs

Instruction Instruction_create(Opcode opcode) {
    // TODO(plesslie): assert on type of opcode
    Instruction result;
    result.opcode = opcode;
    return result;
}

Instruction Instruction_create_dbl(Opcode opcode, double dval) {
    // TODO(plesslie): assert on type of opcode
    Instruction result;
    result.opcode = opcode;
    result.value.dval = dval;
    return result;
}

Instruction Instruction_create_func(
        Opcode opcode, uint32_t func, uint32_t nargs) {
    // TODO(plesslie): assert on type of opcode
    Instruction result;
    result.opcode = opcode;
    result.value.func = func;
    result.value.nargs = nargs;
    return result;
}

struct Interpreter {
    uint8_t     size;
    double      stack[MAX_STACK];
    Instruction prog[MAX_PROGRAM];
};
typedef struct Interpreter Interpreter;

int Interpreter_create(Interpreter *interp) {
    _Static_assert((1u << sizeof(Instruction)*sizeof(interp->size)) > MAX_PROGRAM,
            "pc not large enough to hold max program size");

    interp->size = 0u;
    memset(&interp->stack[0], 0, sizeof(interp->stack[0]) * MAX_STACK);
    memset(&interp->prog[0], 0, sizeof(interp->prog[0]) * MAX_PROGRAM);
    return 0;
}

int Interpreter_destroy(Interpreter *interp) {
    return 0;
}

void Interpreter_push(Interpreter *interp, Instruction instr) {
    interp->prog[interp->size++] = instr;
    assert(interp->size < MAX_PROGRAM);
}

#define ENOHALT     1
#define EINVLDINSTR 2

int Interpreter_run(Interpreter *interp) {
    assert(interp->size > 0u);
    const Instruction *const prog = interp->prog;
    const Instruction *instr;
    const uint8_t size = interp->size;
    double *const stack = interp->stack;
    size_t sp = 0u;
    size_t pc = 0u;

    while (pc < size) {
        instr = &prog[pc++];
        switch (instr->opcode) {
            case OP_NOP:
                break;
            case OP_PUSH:
                assert(sp < MAX_STACK);
                stack[sp++] = DVAL(*instr);
                break;
            case OP_POP:
                assert(sp > 0);
                --sp;
                break;
            case OP_ADD:
                assert(sp >= 2);
                stack[sp-2] = stack[sp-1] + stack[sp-2];
                --sp;
                break;
            case OP_SUB:
                assert(sp >= 2);
                stack[sp-2] = stack[sp-1] - stack[sp-2];
                --sp;
                break;
            case OP_MULT:
                assert(sp >= 2);
                stack[sp-2] = stack[sp-1] * stack[sp-2];
                --sp;
                break;
            case OP_DIV:
                assert(sp >= 2);
                stack[sp-2] = stack[sp-1] / stack[sp-2];
                --sp;
                break;
            case OP_HALT:
                assert(sp == 1);
                return 0;
                break;
            case OP_INVALID:
            default:
                return -EINVLDINSTR;
        }
    }
    return -ENOHALT;
}

double Interpreter_result(Interpreter *interp) {
    return interp->stack[0];
}

int main(int argc, char **argv) {
    _Static_assert(OP_INVALID < (1u << (sizeof(Opcode)*8)), "Too many opcodes");

    int rval;
    Interpreter interp;
    rval = Interpreter_create(&interp);
    if (rval != 0) {
        fprintf(stderr, "Failed to create interpreter!\n");
        exit(EXIT_FAILURE);
    }

    Interpreter_push(&interp, Instruction_create_dbl(OP_PUSH, 8.0));
    Interpreter_push(&interp, Instruction_create_dbl(OP_PUSH, 10.0));
    Interpreter_push(&interp, Instruction_create(OP_DIV));
    Interpreter_push(&interp, Instruction_create(OP_HALT));
    rval = Interpreter_run(&interp);
    if (rval != 0) {
        fprintf(0, "Interpreter run failed with return code: %d\n", rval);
    }

    printf("Result: %f\n", Interpreter_result(&interp));

    rval = Interpreter_destroy(&interp);
    if (rval != 0) {
        fprintf(stderr, "Failed to destroy interpreter!\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
