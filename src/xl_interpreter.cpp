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

    void Interpreter::load(Program&& prog) noexcept {
        this->prog = std::move(prog);
        this->prog.shrink_to_fit();
    }

    bool Interpreter::run() noexcept {
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
                if (stack.empty()) return false;
                stack.pop_back();
                break;
            case EQ: {
                if (stack.size() < 2) return false;
                const double lhs = stack.back();
                stack.pop_back();
                const double rhs = stack.back();
                stack.back() = static_cast<double>(lhs == rhs);
                // stack.pop_back();
                // stack.push_back(static_cast<double>(lhs == rhs));
                break;
            }
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

    double Interpreter::result() const noexcept {
        return stack.back();
    }

    bool Interpreter::check_stack() const noexcept {
        return stack.size() == 1;
    }
    
}
