#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <climits>
#include <cstring>
#include <sstream>
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

stack_emu::runtime_error::runtime_error(const std::string msg_): msg(msg_) {

}
const char * stack_emu::runtime_error::what() const noexcept {
	return msg.c_str();
}

size_t to_int(LongDouble v) {
	assert(v.isInt() && v >= 0);
	stringstream ss;
	ss << v;
	return stoi(ss.str());
}

const stack_emu::vector<command*> commands_do_nothing = {
	new command_begin(), 
	new command_end(),
	new command_label()
};

const stack_emu::vector<command*> commands = {
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

bool stack_emu::emulate(const char* filename) {
	// run
	// read bin
	parser p;
	ifstream stream(filename, ios::binary);
	if (!stream.is_open()) {
		cout << "Cannot open file " << filename << endl;
		return 0;
	}
	p.read(stream);
	stream.close();

	// preset
	#ifdef USE_INT
	LongDouble::default_precision = INT_MAX;
	cout << setprecision(INT_MAX);
	#else
	LongDouble::default_precision = PRECISION / LongDouble::base_exp + 1;
	cout << setprecision(PRECISION);
	#endif

	// emulate
	while (p.active()) {
		std::string start = p.get();
		bool end_ = false;
		bool was = false;
		for (command* i : commands_do_nothing) {
			if (i->check_name(start)) {
				if (i->exec(p) == true) {

					end_ = true;
				}
				was = true;
				break;
			}
		}
		if (!was && p.was_begin == false) {
			continue;
		}
		if (!was && p.was_begin == true) {
			for (command* i : commands) {
				if (i->check_name(start)) {
					if (i->exec(p) == true) {
						end_ = true;
					}
					was = true;
					break;
				}
			}
		}
		if (!was) {
			throw stack_emu::runtime_error("unexpected command " + start);
		}
		if (end_) {
			break;
		}
	}
	if (p.was_begin) {
		throw stack_emu::runtime_error("end command expected");
	}
	for (auto i : commands_do_nothing) {
		delete i;
	}
	for (auto i : commands) {
		delete i;
	}
	return 1;
}
