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
            uint32_t nargs;
        } callop;
    };

    Instruction(Opcode op) : op(op) {}
    Instruction(Opcode op, int ival) : op(op), ival(ival) {}
    Instruction(Opcode op, double dval) : op(op), dval(dval) {}
    Instruction(Opcode op, uint32_t function_idx, uint32_t nargs)
        : op(op) { callop.fidx = function_idx; callop.nargs = nargs; }
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

template<typename T, typename R>
void* evil_cast(R(T::*f)(uint32_t, const double*, const char *&error))
{
    union
    {
        R(T::*pf)(uint32_t, const double*, const char *&error);
        void* p;
    };
    pf = f;
    return p;
}

// TODO: implement
struct MyEnv {
    Result myFunc(/*void *self,*/ uint32_t nargs, const double *stack, const char *&error) {
        error = "this is a really bad error!";
        return std::make_pair(0, stack[0] + stack[1]);
    }
};

typedef void* Callback;

struct Translator {
    virtual ~Translator() {}
    virtual Callback lookup(uint32_t function_idx)=0;
};

struct DummyTranslator : public Translator {
    
    virtual ~DummyTranslator() {}
    Callback lookup(uint32_t function_idx) override {
        //#pragma GCC diagnostic ignored "-Wpmf-conversions"        
        switch (function_idx) {
        case 1:
            return evil_cast(&MyEnv::myFunc);
        default:
            throw std::runtime_error("Unknown function index!");
        }
        //#pragma GCC diagnostic pop
    }
};

typedef std::vector<Instruction> Program;

class JIT {
    const char *error_;
public:
    JIT() : _jit(jit_new_state()), func_(nullptr) {}
    
    ~JIT() {
        jit_destroy_state();
    }

    template <class Environment>
    bool compile(const Program &program, Translator &trans, Environment &env) {
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
                stackPush(JIT_F0);
                const int sp = stack_top_idx - (instr.callop.nargs);
                jit_addi(JIT_R0, JIT_FP, stack_base_offset + sp * sizeof(double));
                
                jit_prepare();
                jit_pushargi((jit_word_t)&env);    // 1st arg: userdata
                jit_pushargi(instr.callop.nargs);  // 2nd arg: # of arguments
                jit_pushargr(JIT_R0);              // 3rd arg: pointer to args on stack
                jit_pushargi((jit_word_t)&error_); // 4th arg: pointer to error message

                auto &&cb = trans.lookup(instr.callop.fidx);
                jit_finishi(reinterpret_cast<jit_pointer_t>(cb));

                stack_top_idx -= instr.callop.nargs; // consume arguments on stack
                jit_retval_d(JIT_F0);
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

    const char *error() const noexcept {
        return error_;
    }
    
private:
    typedef double (*JitFunction)();
    
    jit_state_t *_jit; // DO NOT CHANGE NAME.  Satisfies req.s for GNU lightning macros
    JitFunction func_;
    double result_;
};

  
int main(int argc, char **argv) {
    Program program = {
        { OP_PUSH, 1234. },
        { OP_PUSH, 6666. },
        { OP_CALL, 1, 2  },
        { OP_PUSH, 4444. },
        { OP_SUB         },
        { OP_PUSH, 5555. },
        { OP_MUL         },
    };

    init_jit(argv[0]); // TODO: move to static function
    JIT jit;
    DummyTranslator trans;
    MyEnv env;

    if (!jit.compile(program, trans, env)) {
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

    if (jit.error() != nullptr) {
        fprintf(stderr, "Error: %s\n", jit.error());
    }

    finish_jit();
    
    return 0;
}
