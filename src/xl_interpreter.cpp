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
        return false;
    }

    double Interpreter::result() const {
        return stack.back();
    }
    
}
