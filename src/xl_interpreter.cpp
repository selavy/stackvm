#include "xl_interpreter.h"
#include <utility>
#include "xl_instruction.h"

namespace xl {

    Interpreter::Interpreter()
        : prog()
        , stack()
    {}

    Interpreter::Interpreter(Program&& prog)
        : prog(std::move(prog))
        , stack()
    {
        prog.shrink_to_fit();
    }

    void Interpreter::load(Program&& prog) {
        this->prog = std::move(prog);
        this->prog.shrink_to_fit();
    }

    bool Interpreter::run() {
        size_t ip = 0;
        const size_t len = prog.size();
        while (ip < len) {
            const auto& instr = prog[ip++];
            switch (instr.opcode) {
            case NOP:
                break;
            case PUSH:
                stack.push_back(instr.operand);
                break;
            case POP:
                break;
            case EQ:
                break;
            case NEQ:
                break;
            case PLUS:
                break;
            case MINUS:
                break;
            case MULT:
                break;
            case DIV:
                break;
            case CALL:
                break;
            }
        }

        return check_stack();
    }

    double Interpreter::result() const {
        return stack.back();
    }

    bool Interpreter::check_stack() const {
        return stack.size() == 1;
    }
    
}
