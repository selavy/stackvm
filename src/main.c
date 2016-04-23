#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "instruction.h"
#include "machine.h"

#define LEN (sizeof(program) / sizeof(struct instruction_t))

int main(int argc, char** argv) {
    int64_t result;

    { // PUSH instruction
        struct instruction_t program[] = {
            { .opcode = kPUSH, .operand = 64 }
        };
        assert(machine_execute(&program[0], LEN, &result) == 0);
        assert(result == 64);
    }

    { // POP instruction
        struct instruction_t program[] = {
            { .opcode = kPUSH, .operand = 64 },
            { .opcode = kPOP , .operand = 0  },
            { .opcode = kPUSH, .operand = 27 }
        };
        assert(machine_execute(&program[0], LEN, &result) == 0);
        assert(result == 27);
    }

    { // NOP instruction
        struct instruction_t program[] = {
            { .opcode = kPUSH, .operand = 42 },
            { .opcode = kNOP , .operand = 0  }
        };
        assert(machine_execute(&program[0], LEN, &result) == 0);
        assert(result == 42);
    }

    { // nothing on stack means error
        struct instruction_t program[] = {
            { .opcode = kNOP , .operand = 0  }
        };
        assert(machine_execute(&program[0], LEN, &result) != 0);
    }

    { // too many results on stack means error
        struct instruction_t program[] = {
            { .opcode = kPUSH, .operand = 64 },
            { .opcode = kPUSH, .operand = 42 }
        };
        assert(machine_execute(&program[0], LEN, &result) != 0);
    }

    { // ADD instruction
        struct instruction_t program[] = {
            { .opcode = kPUSH, .operand = 64 },
            { .opcode = kPUSH, .operand = 32 },
            { .opcode = kADD , .operand =  0 }
        };
        assert(machine_execute(&program[0], LEN, &result) == 0);
        assert(result == (64 + 32));
    }

    { // SUB instruction
        struct instruction_t program[] = {
            { .opcode = kPUSH, .operand = 64 },
            { .opcode = kPUSH, .operand = 14 },
            { .opcode = kSUB , .operand =  0 }
        };
        assert(machine_execute(&program[0], LEN, &result) == 0);
        assert(result == (64 - 14));
    }

    { // MUL instruction
        struct instruction_t program[] = {
            { .opcode = kPUSH, .operand = 64 },
            { .opcode = kPUSH, .operand = 20 },
            { .opcode = kMUL , .operand =  0 }
        };
        assert(machine_execute(&program[0], LEN, &result) == 0);
        assert(result == (64 * 20));
    }

    { // DIV instruction
        struct instruction_t program[] = {
            { .opcode = kPUSH, .operand = 64 },
            { .opcode = kPUSH, .operand =  2 },
            { .opcode = kDIV , .operand =  0 }
        };
        assert(machine_execute(&program[0], LEN, &result) == 0);
        assert(result == (64 / 2));
    }

    { /* JNZ instruction - do jump*/
        struct instruction_t program[] = {
            { .opcode = kPUSH, .operand =  1 }, /*0*/ /* TRUE value */
            { .opcode = kJNZ , .operand =  3 }, /*1*/
            { .opcode = kPUSH, .operand =  2 }, /*2*/ /* skipped by jnz */
            { .opcode = kPUSH, .operand = 42 }  /*3*/
        };
        assert(machine_execute(&program[0], LEN, &result) == 0);
        assert(result == 42);
    }

    { /* JNZ instruction - do not jump*/
        struct instruction_t program[] = {
            { .opcode = kPUSH, .operand =  0 }, /*0*/ /* FALSE value */
            { .opcode = kJNZ , .operand =  3 }, /*1*/
            { .opcode = kPUSH, .operand =  2 }, /*2*/
            { .opcode = kPUSH, .operand = 42 }, /*3*/
            { .opcode = kPOP , .operand =  0 }  /*4*/
        };
        assert(machine_execute(&program[0], LEN, &result) == 0);
        assert(result == 2);
    }

    { /* JNZ instruction - jump past end fails*/
        struct instruction_t program[] = {
            { .opcode = kPUSH, .operand =  1 }, /*0*/ /* TRUE value */
            { .opcode = kJNZ , .operand =  9 }, /*1*/ /* try to jump past end of instructions */
            { .opcode = kPUSH, .operand =  2 }, /*2*/ /* skipped by jnz */
        };
        assert(machine_execute(&program[0], LEN, &result) != 0);
    }

    printf("Passed.\n");
    return 0;
}
