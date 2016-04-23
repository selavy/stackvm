#include <stdio.h>
#include <stdint.h>
#include "instruction.h"
#include "machine.h"

int main(int argc, char** argv) {
    struct instruction_t program[] = {
        { .opcode = kPUSH, .operand = 64 }
    };


    int64_t result;
    if (machine_execute(&program[0], 1, &result) == 0) {
        printf("Result = %llu\n", result);
    } else {
        printf("Interpreter failed!\n");
    }

    return 0;
}
