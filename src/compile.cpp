#include <stack.hpp>
#include <arithmetic.hpp>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <climits>
#include <cstring>
#include <vector.hpp>
#include <algorithm>
#include <stack_emu.hpp>
#include <parser.hpp>
#include <command.hpp>

using namespace stack_emu;
using namespace arithmetic;
using std::endl;
using std::string;
using std::cin;
using std::cout;

stack_emu::compile_error::compile_error(const std::string msg_): msg(msg_) {

}
const char * stack_emu::compile_error::what() const noexcept {
	return msg.c_str();
}

const stack_emu::vector<command*> commands = {
	new command_begin(), 
	new command_end(),
	new command_label(),
	new command_jmp(),
	new command_jeq(),
	new command_jne(),
	new command_ja(),
	new command_jae(),
	new command_jb(),
	new command_jbe(),
	new command_push(),
	new command_pop(),
	new command_pushr(),
	new command_popr(),
	new command_add(),
	new command_sub(),
	new command_mul(),
	new command_div(),
	#ifndef USE_INT
	new command_sqrt(),
	#endif
	new command_in(),
	new command_out(),
	new command_call(),
	new command_ret()
};

bool stack_emu::compile(const char* filename, const char* dest) {

	// open file
	parser p;
	ifstream stream;
	stream.open(filename);
	if (!stream.is_open()) {
		cout << "Cannot open file " << filename << endl;
		return 0;
	}
	// compile
	p.compile(stream);
	stream.close();
	
	// presets
	#ifdef USE_INT
	LongDouble::default_precision = INT_MAX;
	cout << setprecision(INT_MAX);
	#else
	LongDouble::default_precision = PRECISION / LongDouble::base_exp + 1;
	cout << setprecision(PRECISION);
	#endif

	// parse
	while (p.active()) {

		std::string start = p.get();
		bool was = false;
		for (command* i : commands) {
			if (i->check_name(start)) {
				i->compile(p);
				was = true;
				break;
			}
		}
		if (!was) {
			throw stack_emu::compile_error("unexpected command " + start);
		}
	}

	ofstream out(dest, ios::binary);
	if (!out.is_open()) {
		cout << "Cannot write file " << dest << endl;
		return 0;
	}
	for (size_t i = 0; i < p.commands.size(); i++) {
		out.write(p.commands[i].data(), p.commands[i].size() * sizeof(char));
		if (i + 1 < p.commands.size()) {
			out.write(DELIMITER.data(), 1 * sizeof(char));
		}
	}
	for (auto i : commands) {
		delete i;
	}
	return 1;
}
