#ifndef XL_INTERPRETER__H_
#define XL_INTERPRETER__H_

#include <vector>

namespace xl {
    class Instruction;
}

namespace xl {
    
    typedef std::vector<Instruction> Program;
    
    class Interpreter final {
    public:
        typedef std::vector<double> stack_type;
        
    public:
        Interpreter();
        Interpreter(Program&& prog);
        void load(Program&& prog) noexcept;
        bool run() noexcept;
        double result() const noexcept;

    private:
        bool check_stack() const noexcept;
        
    private:
        Program    prog;
        stack_type stack;
    };
    
}

#endif // XL_INTERPRETER__H_
