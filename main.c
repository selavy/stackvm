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
    OP_CALL,
};

const char *CODESTR[] = {
    "NOP",
    "PUSH",
    "ADD",
    "SUB",
    "MUL",
    "DIV",
    "CALL",
};
const char *trans_opcode(int op) {
    switch (op) {
    case OP_NOP:
        return "NOP";
    case OP_PUSH:
        return "PUSH";
    case OP_ADD:
        return "ADD";
    case OP_SUB:
        return "SUB";
    case OP_MUL:
        return "MUL";
    case OP_DIV:
        return "DIV";
    case OP_CALL:
        return "CALL";
    default:
        return "unknown";
    }
        
}

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

double fixme_function(void *env, uint32_t narg/*, uint64_t stackp*/) {
    //printf("fixme function!!!! stackp = %p\n", (void*)stackp);
    printf("fixme function!!!\n");
    printf("\tnarg = %d\n", narg);
    printf("\tenv = %p\n", env);
    return 1.0;
}

jit_node_t *JIT_translate(jit_state_t *_jit, const struct instruction *restrict program, size_t progsz, void *env) {
    const struct instruction *ip;    
    size_t pc = 0;
    int sp;
    int fp;
    jit_node_t *fn;

    fn = jit_note(NULL, 0);
    jit_prolog();
    fp = sp = jit_allocai(32 * sizeof(double));
    
    while (pc < progsz) {
        ip = &program[pc];
        printf("BEFORE %s: fp: %d, sp: %d\n", trans_opcode(ip->op), fp, sp);
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
        case OP_CALL:
            jit_prepare();
            jit_pushargi((jit_word_t)env);
            jit_pushargi(ip->callop.narg);

            _JIT_stack_push(_jit, JIT_F0, &sp); // head of stack is stored in a register
            
            /* jit_addr(JIT_R0, JIT_FP, sp*sizeof(double)); */
            /* jit_pushargr(JIT_R0); */
            
            //jit_finishi((jit_pointer_t)ENV_translate_idx(ip->callop.fidx));
            jit_finishi((jit_pointer_t)fixme_function);

            sp -= sizeof(double) * ip->callop.narg;

            jit_retval_d(JIT_F0);
            break;
        }

        printf("AFTER %s: fp: %d, sp: %d\n", trans_opcode(ip->op), fp, sp);
        ++pc;
    }

    printf("\n------- end of jitting ------ \n");
    printf("fp = %d\n", fp);
    printf("sp = %d\n", sp);

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
        { .op=OP_CALL, .callop={ .fidx=1, .narg=1 } },
        /* { .op=OP_PUSH, .dval=5. }, */
        /* { .op=OP_MUL            }, */
        /* { .op=OP_PUSH, .dval=2. }, */
        /* { .op=OP_DIV            }, */
    };
    jit_state_t *_jit;
    jit_node_t *fn;
    typedef double (*func)(void);
    func my_func;
    int *env;

    init_jit(argv[0]);
    _jit = jit_new_state();

    env = malloc(sizeof(int));
    printf("address of env: %p\n", env);
    
    fn = JIT_translate(_jit, &program[0], ARRSIZE(program), env);
    (void)jit_emit();

    my_func = (func)jit_address(fn);

    jit_disassemble();

    printf("result: %f\n", my_func());

    jit_destroy_state();
    finish_jit();

    free(env);
    
    return 0;
}
