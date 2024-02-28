#include <parser.hpp>
#include <command.hpp>
#include <stack_emu.hpp>

namespace stack_emu {

	void check_register_name(const string& reg) {
		#ifdef CHECK_REGISTER_NAME
		if (!((reg.size() == 2 && reg[0] >= 'a' && reg[0] < 'a' + MAX_REGISTERS && reg[1] == 'x') || reg == "pc")) {
			throw compile_error("invalid reg: " + reg + ", register name should be pc or ax or bx or cx, etc... up to " + to_string(MAX_REGISTERS));
		}
		#endif
	}

	void check_register_size(const stack_emu::vector<pair<string, size_t>>& registers) {
		if (registers.size() > MAX_REGISTERS) {
			compile_error("there are only " + to_string(MAX_REGISTERS) + " registers");
		}
	}

	parser::parser() {
		was_begin = false;
		current_command = 0;
	}

	void parser::read(std::ifstream& stream) {
		commands = stack_emu::vector<std::string>();
		commands.push_back("");
		char r[1];
		while (stream.read(r, 1), stream) {
			if (r[0] == DELIMITER[0]) {
				commands.push_back("");
			} else {
				commands.back() += r[0];
			}
		}
		if (commands.size() == 1 && commands.back().size() == 0) {
			commands.pop_back();
		}
	}

	size_t parser::get_label(std::string label) {
		size_t idx = lower_bound(labels.data(), labels.data() + labels.size(), 
			pair<string, size_t>{label, 0}) - labels.data();
		if (idx >= labels.size() || labels[idx].first != label) {
			return SIZE_MAX;
		}
		return labels[idx].second;
	}

	size_t parser::get_register(std::string reg) {
		size_t idx = lower_bound(registers.data(), registers.data() + registers.size(), 
			pair<string, size_t>{reg, 0}) - registers.data();
		if (idx >= registers.size() || registers[idx].first != reg) {
			return SIZE_MAX;
		}
		return registers[idx].second;
	}

	void parser::compile(std::ifstream& stream) {
		string inp;
		while (stream >> inp) { // read file
			std::transform(inp.begin(), inp.end(), inp.begin(),
					[](unsigned char c){ return std::tolower(c); });
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

		for (size_t i = 0; i < commands.size(); i++) {
			if (command_label().check_name(commands[i])) {
				if (get_label(commands[i].substr(0, commands[i].size() - 1)) != SIZE_MAX) {
					throw stack_emu::compile_error("label declare should be unique " + commands[i].substr(0, commands[i].size() - 1));
				}
				labels.push_back({commands[i].substr(0, commands[i].size() - 1), i + 1});
				commands[i] = to_string(i + 1) + ":";
			}
		}
		for (size_t i = 0; i < commands.size(); i++) {
			if (commands[i] == command_pushr().name || commands[i] == command_popr().name) {
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
		std::sort(labels.data(), labels.data() + labels.size());
		std::sort(registers.data(), registers.data() + registers.size());
	}

	std::string parser::get() {
		return commands[current_command++];
	}
	bool parser::active() {
		return current_command < commands.size();
	}
}