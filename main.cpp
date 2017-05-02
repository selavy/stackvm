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

typedef void* Callback;

struct Translator {
    virtual ~Translator() {}
    virtual Callback lookup(uint32_t function_idx)=0;
};

struct IF {
    Result handleIf(uint32_t nargs, const double *stack, const char *&err) {
        if (nargs < 3) {
            err = "IF: not enough arguments";
            return std::make_pair(false, 0.);
        }
        printf("\thandleIf(%p)\n", this);        
        return std::make_pair(true, stack[0] != 0. ? stack[1] : stack[2]);
    }
};
    
struct MIN {
    Result handleMin(uint32_t nargs, const double *stack, const char *&err) {
        if (nargs < 1) {
            err = "MIN: not enough arguments";
            return std::make_pair(false, 0.);
        }
        printf("\thandleMin(%p)\n", this);        
        double result = stack[0];
        for (int i = 1; i < nargs; ++i) {
            result = std::min(result, stack[i]);
        }
        return std::make_pair(true, result);
    }
};

struct BotSvc {
    BotSvc() : vals(new int[5]) {
        for (int i = 0; i < 5; ++i) {
            vals[i] = i;
        }
    }
    ~BotSvc() {
        delete[] vals;
    }
    Result handleQuote(uint32_t nargs, const double *stack, const char *&err) {
        if (nargs < 1) {
            err = "QUOTE: too few arguments";
            return std::make_pair(false, 0.);
        }
        printf("\thandleQuote(%p)\n", this);
        const double result = stack[0] > 0. ? vals[0] : vals[1];
        return std::make_pair(true, result);
    }

    int *vals;
};

enum {
    eIf = 1,
    eMin = 2,
    eQuote = 3,
};

template <class Impl>
struct Env : IF, MIN, public Impl, public Translator {
    virtual ~Env() {}
    Callback lookup(uint32_t function_idx) override {
        switch (function_idx) {
        case eIf:
            return evil_cast(&IF::handleIf);
        case eMin:
            return evil_cast(&MIN::handleMin);
        case eQuote:
            return evil_cast(&Impl::handleQuote);
        default:
            throw std::runtime_error("Unknown function index!");
        }
    }
};

typedef std::vector<Instruction> Program;

class JIT {
    const char *error_;
    const uint64_t JIT_ERROR;
    
public:
    JIT() : _jit(jit_new_state()), func_(nullptr), JIT_ERROR(78787878ull) {}
    
    ~JIT() {
        jit_destroy_state();
    }

    template <class Environment>
    bool compile(const Program &program, Translator &trans, Environment &env) {
        jit_node_t *ref;        
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
                jit_retval(JIT_R0);

                ref = jit_bnei(JIT_R0, 0);
                jit_reti_d(*reinterpret_cast<const double*>(&JIT_ERROR));
                jit_patch(ref);
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
        //return true;
        return *reinterpret_cast<uint64_t*>(&result_) != JIT_ERROR;
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
        { OP_PUSH, 1. },
        { OP_PUSH, 2. },
        { OP_PUSH, 3. },
        { OP_CALL, eIf, 3  },
        { OP_CALL, eQuote, 1 },
    };

    init_jit(argv[0]); // TODO: move to static function
    JIT jit;
    Env<BotSvc> env;

    printf("\nenv = %p\n", &env);

    if (!jit.compile(program, env, env)) {
        std::cerr << "Failed to compile program!" << std::endl;
        return 1;
    }

    if (!jit.run()) {
        std::cerr << "Error running program: " << jit.error() << std::endl;
        return 1;
    }

    //std::cout << "\n\nProgram: \n";
    //jit.disassemble();

    std::cout << "Result: " << jit.result() << std::endl;
    std::cout << "Expected: " << (((1234. + 6666.) - 4444.) * 5555.) << std::endl;

    finish_jit();
    
    return 0;
}
