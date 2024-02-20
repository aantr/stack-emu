#include <iostream>
#include <stack_emu.hpp>

int main(int argc, char* argv[]) {

	if (argc != 3 || !(argv[1] == std::string("compile") || argv[1] == std::string("emulate") || argv[1] == std::string("compulate"))) {
		std::cout << "Usage: {executable} {compile|emulate|compulate (both)} {path_to_file}" << std::endl;
	} else if (argv[1] == std::string("compile")) {
		std::string dest = std::string(argv[2]) + ".out";
		if (stack_emu::compile(argv[2], dest.c_str())) {
			std::cout << dest << std::endl;
		}
	} else if (argv[1] == std::string("emulate")){
		stack_emu::emulate(argv[2]);
	} else {
		std::string dest = std::string(argv[2]) + ".out";
		if (stack_emu::compile(argv[2], dest.c_str())) {
			std::cout << dest << std::endl;
			stack_emu::emulate(dest.c_str());
		}
	}
	return 0;

}