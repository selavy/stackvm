#include <iostream>
#include <vector>
#include <utility>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
extern "C" {
#include <lightning.h>
}

enum Opcode {
    OP_NOP,
    OP_PUSH,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_CALL,
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

struct Instruction {
    uint8_t op;
    union {
        int    ival;
        double dval;
        struct {
            uint32_t fidx;
            uint32_t narg;
        } callop;
    };

    Instruction(Opcode op) : op(op) {}
    Instruction(Opcode op, int ival) : op(op), ival(ival) {}
    Instruction(Opcode op, double dval) : op(op), dval(dval) {}
    Instruction(Opcode op, uint32_t function_idx, uint32_t nargs)
        : op(op) { callop.fidx = function_idx; callop.narg = nargs; }
};

void _JIT_stack_push(jit_state_t *_jit, int reg, int *sp) {
    jit_stxi_d(*sp, JIT_FP, reg);
    *sp += sizeof(double);
}

void _JIT_stack_pop(jit_state_t *_jit, int reg, int *sp) {
    *sp -= sizeof(double);
    jit_ldxi_d(reg, JIT_FP, *sp);
}

typedef std::pair<int, double> Result;

Result fixme_function(void *env, uint32_t nargs, const double *stack) {
    return std::make_pair(0, stack[0] + stack[1]);
}

Result second_function(void *env, uint32_t nargs, const double *stack) {
    int retcode;
    double result;
    if (nargs < 3) {
        retcode = 1;
        result = 0.0;
    } else {
        retcode = 0;
        result = stack[0] != 0.0 ? stack[1] : stack[2];
    }
    return std::make_pair(retcode, result);
}

typedef Result (*Callback)(void *, uint32_t, const double*);

struct Translator {
    virtual ~Translator() {}
    virtual Callback lookup(uint32_t function_idx)=0;
};
struct DummyTranslator : public Translator {
    virtual ~DummyTranslator() {}
    Callback lookup(uint32_t function_idx) override {
        switch (function_idx) {
        case 1:
            return fixme_function;
        case 2:
            return second_function;
        default:
            throw std::runtime_error("Unknown function index!");
        }
    }
};

typedef std::vector<Instruction> Program;

class JIT {
public:
    JIT() : _jit(jit_new_state()), func_(nullptr) {}
    
    ~JIT() {
        jit_destroy_state();
    }

    bool compile(const Program &program, Translator &trans, void *userdata) {
        jit_node_t *fn = jit_note(NULL, 0);
        jit_prolog();
        const int stack_base_offset = jit_allocai(32 * sizeof(double));
        int stack_top_idx = 0;

        auto &&stackPush = [&](int reg) {
            jit_stxi_d(stack_base_offset + stack_top_idx * sizeof(double), JIT_FP, reg);
            ++stack_top_idx;
        };
        
        auto &&stackPop = [&](int reg) {
            --stack_top_idx;
            jit_ldxi_d(reg, JIT_FP, stack_base_offset + stack_top_idx * sizeof(double));
            return reg;
        };

        size_t pc = 0;
        const size_t progsz = program.size();
        while (pc < progsz) {
            const Instruction &instr = program[pc];
            switch (instr.op) {
            case OP_NOP:
                break;
            case OP_PUSH:
                stackPush(JIT_F0);
                jit_movi_d(JIT_F0, instr.dval);
                break;
            case OP_ADD: {
                const int reg = stackPop(JIT_F1);
                jit_addr_d(JIT_F0, reg, JIT_F0);
            }
                break;
            case OP_SUB: {
                const int reg = stackPop(JIT_F1);
                jit_subr_d(JIT_F0, reg, JIT_F0);
            }
                break;
            case OP_MUL: {
                const int reg = stackPop(JIT_F1);
                jit_mulr_d(JIT_F0, reg, JIT_F0);
            }
                break;
            case OP_DIV: {
                const int reg = stackPop(JIT_F1);
                jit_divr_d(JIT_F0, reg, JIT_F0);                
            }
                break;
            case OP_CALL: {
            }
                break;
            }

            ++pc;
        }

        jit_retr_d(JIT_F0);
        jit_epilog();

        (void)jit_emit();
        func_ = reinterpret_cast<JitFunction>(jit_address(fn));
        
        return true;
    }

    bool run() {
        result_ = func_();
        return true;
    }

    double result() const noexcept {
        return result_;
    }

    void disassemble() {
        jit_disassemble();
    }

    const char *error() const {
        return "TODO: implement";
    }
    
private:
    typedef double (*JitFunction)();
    
    jit_state_t *_jit; // DO NOT CHANGE NAME.  Satisfies req.s for GNU lightning macros
    JitFunction func_;
    double result_;
};

// TODO: implement
struct MyEnv {};

// jit_node_t *JIT_translate(jit_state_t *_jit, const struct instruction *restrict program, size_t progsz, void *env, translator_t trans) {
//     const struct instruction *ip;    
//     size_t pc = 0;
//     int sp;
//     jit_node_t *fn;
//     jit_node_t *ref;
//     callback_t cb;

//     fn = jit_note(NULL, 0);
//     jit_prolog();
//     sp = jit_allocai(6 * sizeof(double));

//     while (pc < progsz) {
//         ip = &program[pc];
//         switch (ip->op) {
//         case OP_NOP:
//             break;
//         case OP_PUSH:
//             _JIT_stack_push(_jit, JIT_F0, &sp);
//             jit_movi_d(JIT_F0, ip->dval);
//             break;
//         case OP_ADD:
//             _JIT_stack_pop(_jit, JIT_F1, &sp);
//             jit_addr_d(JIT_F0, JIT_F1, JIT_F0);
//             break;
//         case OP_SUB:
//             _JIT_stack_pop(_jit, JIT_F1, &sp);
//             jit_subr_d(JIT_F0, JIT_F1, JIT_F0);            
//             break;
//         case OP_MUL:
//             _JIT_stack_pop(_jit, JIT_F1, &sp);
//             jit_mulr_d(JIT_F0, JIT_F1, JIT_F0);            
//             break;
//         case OP_DIV:
//             _JIT_stack_pop(_jit, JIT_F1, &sp);
//             jit_divr_d(JIT_F0, JIT_F1, JIT_F0);            
//             break;
//         case OP_CALL:
//             jit_prepare();
//             jit_pushargi((jit_word_t)env); // 1st argument is env pointer
//             jit_pushargi(ip->callop.narg); // 2nd argument is number of arguments

//             _JIT_stack_push(_jit, JIT_F0, &sp); // move top of stack to C stack
//             jit_addi(JIT_R0, JIT_FP, sp - (ip->callop.narg * sizeof(double))); // 3rd argument is stack pointer
//             jit_pushargr(JIT_R0);

//             cb = trans(ip->callop.fidx);
//             if (!cb) {
//                 printf("Failed to map function index: %d\n", ip->callop.fidx);
//                 abort();
//             }
//             jit_finishi((jit_pointer_t)cb);
            
//             sp -= sizeof(double) * ip->callop.narg; // consume arguments on stack
            
//             jit_retval_d(JIT_F0);
//             jit_retval(JIT_R0);

//             ref = jit_beqi(JIT_R0, 0);
            
//             // return early because of bad retcode
//             jit_movi_d(JIT_F0, -1.0);
//             jit_retr_d(JIT_F0);
//             jit_patch(ref);
            
//             break;
//         }

//         ++pc;
//     }

//     jit_retr_d(JIT_F0);
//     jit_epilog();

//     return fn;
// }

  
int main(int argc, char **argv) {
    Program program = {
        { OP_PUSH, 1234. },
        { OP_PUSH, 6666. },
        { OP_ADD         },
        { OP_PUSH, 4444. },
        { OP_SUB         },
        { OP_PUSH, 5555. },
        { OP_MUL         },
    };

    init_jit(argv[0]); // TODO: move to static function
    JIT jit;
    DummyTranslator trans;
    MyEnv env;

    if (!jit.compile(program, trans, &env)) {
        std::cerr << "Failed to compile program!" << std::endl;
        return 1;
    }

    if (!jit.run()) {
        std::cerr << "Error running program: " << jit.error() << std::endl;
        return 1;
    }

    std::cout << "\n\nProgram: \n";
    jit.disassemble();

    std::cout << "Result: " << jit.result() << std::endl;
    std::cout << "Expected: " << (((1234. + 6666.) - 4444.) * 5555.) << std::endl;

    finish_jit();

    
    return 0;
}
