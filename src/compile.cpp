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

const stack_emu::vector<string> null_commands = {"begin", "end", "pop", "add", "sub", "mul", "div", "in", "out", "ret"};

void check_register_name(const string& reg) {
	#ifdef CHECK_REGISTER_NAME
	if (!(reg.size() == 2 && reg[0] >= 'a' && reg[0] < 'a' + MAX_REGISTERS && reg[1] == 'x')) {
		throw compile_error("register name should be ax or bx or cx, etc... up to " + to_string(MAX_REGISTERS));
	}
	#endif
}

void check_register_size(const stack_emu::vector<pair<string, size_t>>& registers) {
	if (registers.size() > MAX_REGISTERS) {
		compile_error("there are only " + to_string(MAX_REGISTERS) + " registers ");
	}
}

bool stack_emu::compile(const char* filename, const char* dest) {

	// open file
	ifstream stream;
	stream.open(filename);
	if (!stream.is_open()) {
		cout << "Cannot open file " << filename << endl;
		return 0;
	}
	
	// presets
	string inp;
	#ifdef USE_INT
	LongDouble::default_precision = INT_MAX;
	cout << setprecision(INT_MAX);
	#else
	LongDouble::default_precision = PRECISION / LongDouble::base_exp + 1;
	cout << setprecision(PRECISION);
	#endif
	stack_emu::vector<string> commands;
	while (stream >> inp) { // read file
		size_t idx = inp.find("//");
		if (idx == std::string::npos) {
			commands.push_back(inp);
		} else {
			if (idx) {
				commands.push_back(inp.substr(0, idx));
			}
			getline(stream, inp);
		}
	}

	stream.close();

	stack_emu::vector<pair<string, size_t>> labels;
	stack_emu::vector<pair<string, size_t>> registers;
	auto get_label = [&](string label) -> size_t { // index or SIZE_MAX
		size_t idx = lower_bound(labels.data(), labels.data() + labels.size(), 
			pair<string, size_t>{label, 0}) - labels.data();
		if (idx >= labels.size() || labels[idx].first != label) {
			return SIZE_MAX;
		}
		return labels[idx].second;
	};
	auto get_register = [&](string reg) -> size_t { // index or SIZE_MAX
		size_t idx = lower_bound(registers.data(), registers.data() + registers.size(), 
			pair<string, size_t>{reg, 0}) - registers.data();
		if (idx >= registers.size() || registers[idx].first != reg) {
			return SIZE_MAX;
		}
		return registers[idx].second;
	};
	for (size_t i = 0; i < commands.size(); i++) {
		if (commands[i].back() == ':') {
			if (get_label(commands[i].substr(0, commands[i].size() - 1)) != SIZE_MAX) {
				throw stack_emu::compile_error("label declare should be unique " + commands[i].substr(0, commands[i].size() - 1));
			}
			labels.push_back({commands[i].substr(0, commands[i].size() - 1), i + 1});
			commands[i] = to_string(i + 1) + ":";
		}
	}
	for (size_t i = 0; i < commands.size(); i++) {
		if (commands[i] == "pushr" || commands[i] == "popr") {
			if (i == commands.size() - 1) {
				throw stack_emu::compile_error(commands[i] + ": expected input");
			}
			if (get_register(commands[i + 1]) != SIZE_MAX) {
				commands[i + 1] = to_string(get_register(commands[i + 1]));
			} else {
				registers.push_back({commands[i + 1], registers.size()});
				check_register_name(commands[i + 1]);
				check_register_size(registers);
				commands[i + 1] = to_string(registers.size() - 1);
			}
		}
	}
	sort(labels.data(), labels.data() + labels.size());
	sort(registers.data(), registers.data() + registers.size());

	// parse
	size_t current_command = 0;
	while (current_command < commands.size()) {
		inp = commands[current_command++];
		std::transform(inp.begin(), inp.end(), inp.begin(),
				[](unsigned char c){ return std::tolower(c); });
		bool null_command = false;
		for (auto i : null_commands) {
			null_command |= i == inp;
		}
		if (null_command) {
			// nothing here
		} else if (inp.back() == ':') {

		} else if (inp == "jmp" || inp == "jeq" || inp == "jne" || inp == "ja" || inp == "jb" || inp == "jae" || inp == "jbe") {
			if (current_command == commands.size()) {
				throw stack_emu::compile_error(inp + ": expected input");
			}
			string lab = commands[current_command++];
			if (get_label(lab) == SIZE_MAX) {
				throw stack_emu::compile_error(inp + ": no such label " + lab);
			}
			commands[current_command - 1] = to_string(get_label(lab));
		} else if (inp == "push") {
			if (current_command == commands.size()) {
				throw stack_emu::compile_error(inp + ": expected input");
			}
			string value = commands[current_command++];
			LongDouble v;
			bool error = false;
			try {
				v = value;
			} catch (const InitStringError &e) {
				error = true;
			}
			#ifdef USE_INT
			if (error || !v.isInt()) {
				throw stack_emu::compile_error(inp + ": wrong value format: " + value + ", expected: [-+]?[0-9]*");
			}
			#else 
			if (error) {
				throw stack_emu::compile_error(inp + ": wrong value format: " + value + ", expected: [-+]?[0-9]*[.]?[0-9]*");
			}
			#endif
		} else if (inp == "pushr") {
			if (current_command == commands.size()) {
				throw stack_emu::compile_error(inp + ": expected input");
			}
			string reg = commands[current_command++];
		} else if (inp == "popr") {
			if (current_command == commands.size()) {
				throw stack_emu::compile_error(inp + ": expected input");
			}
			string reg = commands[current_command++];
		}
		#ifndef USE_INT
		else if (inp == "sqrt") {

		}
		#endif
		else if (inp == "call") {
			if (current_command == commands.size()) {
				throw stack_emu::compile_error(inp + ": expected input");
			}
			string lab = commands[current_command++];
			if (get_label(lab) == SIZE_MAX) {
				throw stack_emu::compile_error(inp + ": no such label " + lab);
			}
			commands[current_command - 1] = to_string(get_label(lab));
		} else {
			throw stack_emu::compile_error("unexpected command " + inp);
		}
	}

	ofstream out(dest, ios::binary);
	if (!out.is_open()) {
		cout << "Cannot write file " << dest << endl;
		return 0;
	}
	for (size_t i = 0; i < commands.size(); i++) {
		out.write(commands[i].data(), commands[i].size() * sizeof(char));
		if (i + 1 < commands.size()) {
			out.write(DELIMITER.data(), 1 * sizeof(char));
		}
	}
	return 1;
}