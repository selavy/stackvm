#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <lightning.h>

enum opcode {
    OP_NOP,
    OP_PUSH,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
};

struct instruction {
    uint8_t op;
    union {
        int    ival;
        double dval;
        struct {
            uint32_t fidx;
            uint32_t narg;
        } callop;
    };
};

void _JIT_stack_push(jit_state_t *_jit, int reg, int *sp) {
    jit_stxi_d(*sp, JIT_FP, reg);
    *sp += sizeof(double);
}

void _JIT_stack_pop(jit_state_t *_jit, int reg, int *sp) {
    *sp -= sizeof(double);
    jit_ldxi_d(reg, JIT_FP, *sp);
}

jit_node_t *JIT_translate(jit_state_t *_jit, const struct instruction *restrict program, size_t progsz) {
    const struct instruction *ip;    
    size_t pc = 0;
    int sp;
    jit_node_t *fn;

    fn = jit_note(NULL, 0);
    jit_prolog();
    sp = jit_allocai(32 * sizeof(int));
    
    while (pc < progsz) {
        ip = &program[pc];
        switch (ip->op) {
        case OP_NOP:
            break;
        case OP_PUSH:
            _JIT_stack_push(_jit, JIT_F0, &sp);
            jit_movi_d(JIT_F0, ip->dval);
            break;
        case OP_ADD:
            _JIT_stack_pop(_jit, JIT_F1, &sp);
            jit_addr_d(JIT_F0, JIT_F1, JIT_F0);
            break;
        case OP_SUB:
            _JIT_stack_pop(_jit, JIT_F1, &sp);
            jit_subr_d(JIT_F0, JIT_F1, JIT_F0);            
            break;
        case OP_MUL:
            _JIT_stack_pop(_jit, JIT_F1, &sp);
            jit_mulr_d(JIT_F0, JIT_F1, JIT_F0);            
            break;
        case OP_DIV:
            _JIT_stack_pop(_jit, JIT_F1, &sp);
            jit_divr_d(JIT_F0, JIT_F1, JIT_F0);            
            break;
        }
        
        ++pc;
    }

    jit_retr_d(JIT_F0);
    jit_epilog();
    return fn;
}

#define ARRSIZE(arr) (sizeof(arr) / sizeof(*arr))
  
int main(int argc, char **argv) {
    struct instruction program[] = {
        { .op=OP_PUSH, .dval=1. },
        { .op=OP_PUSH, .dval=2. },
        { .op=OP_ADD            },
    };
    jit_state_t *_jit;
    jit_node_t *fn;
    typedef double (*func)(void);
    func my_func;

    init_jit(argv[0]);
    _jit = jit_new_state();

    fn = JIT_translate(_jit, &program[0], ARRSIZE(program));
    (void)jit_emit();

    my_func = (func)jit_address(fn);

    printf("result: %f\n", my_func());

    jit_destroy_state();
    finish_jit();
    
    return 0;
}
