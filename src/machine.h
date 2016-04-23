#ifndef MACHINE__H_
#define MACHINE__H_

#include <stdint.h>

struct instruction_t;

int machine_execute(const struct instruction_t* program, int32_t len, int64_t* result);

#endif // MACHINE__H_
