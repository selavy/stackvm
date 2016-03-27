#include <iostream>
#include <boost/program_options.hpp>
#include "command_line.h"

int main(int argc, char *argv[]) {
    const auto vm = parse_cmdline(argc, argv); 
    return 0;
}
