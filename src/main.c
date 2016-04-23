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
    printf("Passed.\n");
    return 0;
}
