#pragma once

#include <vector.hpp>
#include <stack.hpp>
#include <stack_emu.hpp>
#include <arithmetic.hpp>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>

namespace stack_emu {

	class parser {
	public:
		stack_emu::stack<arithmetic::LongDouble> st;
		#ifdef USE_CALL_STACK
		stack_emu::stack<size_t> call_st;
		#endif
		vector<arithmetic::LongDouble> reg;
		bool was_begin;

		vector<std::string> commands;
		size_t current_command;

		vector<std::pair<std::string, size_t>> labels;
		vector<std::pair<std::string, size_t>> registers;

		void compile(std::ifstream& stream);
		size_t get_label(std::string label);
		size_t get_register(std::string reg);

		parser();
		void read(std::ifstream&);
		std::string get();
		bool active();
		~parser() = default;
	};
}