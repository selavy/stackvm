#include "command_line.h"
#include <iostream> // std::cout
#include <cstdlib>  // std::exit()
#include <boost/program_options.hpp>

boost::program_options::options_description get_descriptions()
{
    namespace po = boost::program_options;
    
    po::options_description desc("Allowed options");
    // clang-format off
    desc.add_options()
        ("help,h", "produce help message")
        ;
    // clang-format on
    return desc;
}


boost::program_options::variables_map parse_cmdline(int argc, char *argv[]) {
    namespace po = boost::program_options;
    using namespace std;
    
    po::options_description desc = get_descriptions();
    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch (const std::exception& ex) {
        cerr << ex.what() << "\n"
             << desc << endl;
        ::exit(1);
    }
    if (vm.count("help")) {
        cout << desc << '\n';
        ::exit(0);
    }
    return vm;    
}
