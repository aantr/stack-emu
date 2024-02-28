#pragma once

#include <stack_emu.hpp>
#include <parser.hpp>
#include <arithmetic.hpp>

using namespace arithmetic;

#define GETLAB if (!p.active()) { throw stack_emu::runtime_error(name + ": expected input"); } std::string lab = p.get();
#define GETVALUE if (!p.active()) { throw stack_emu::runtime_error(name + ": expected input"); } std::string value = p.get();
#define GETAB if (p.st.size() < 2) { throw stack_emu::runtime_error(name + ": expected atleast 2 elements in stack"); } \
arithmetic::LongDouble b, a; b = p.st.top(); p.st.pop(); a = p.st.top(); p.st.pop();
#ifdef USE_INT
#define CHECKV bool error = false; try { v = value; } catch (const arithmetic::InitStringError &e) { error = true; } \
if (error) { \
	throw compile_error(name + ": wrong value format: " + value + ", expected: [-+]?[0-9]*[.]?[0-9]*"); \
}
#else
#define CHECKV bool error = false; try { v = value; } catch (const arithmetic::InitStringError &e) { error = true; } \
if (error) { \
	throw compile_error(name + ": wrong value format: " + value + ", expected: [-+]?[0-9]*[.]?[0-9]*"); \
}
#endif
#ifdef USE_INT
#define CHECKV_RE bool error = false; try { v = value; } catch (const arithmetic::InitStringError &e) { error = true; } \
if (error) { \
	throw runtime_error(name + ": wrong value format: " + value + ", expected: [-+]?[0-9]*[.]?[0-9]*"); \
}
#else
#define CHECKV_RE bool error = false; try { v = value; } catch (const arithmetic::InitStringError &e) { error = true; } \
if (error) { \
	throw runtime_error(name + ": wrong value format: " + value + ", expected: [-+]?[0-9]*[.]?[0-9]*"); \
}
#endif

namespace stack_emu {
	class command {
	public:
		std::string name = "default";

		virtual bool check_name(std::string &name_) const {
			return name_ == name;
		};

		virtual bool compile(parser &p) const {
			if (!p.was_begin) {
 				// do nothing by default
			}
			return 0;
		};

		virtual bool exec(parser &p) const {
			if (!p.was_begin) {
 				// do nothing by default
			}
			return 0;
		};

		virtual ~command() = default;
	};

	class command_begin: public command {
	public:
		command_begin() {
			name = "begin";
		}
		bool exec(parser& p) const override {
			if (p.was_begin) {
				throw stack_emu::runtime_error("double begin command");
			}
			p.was_begin = true;
			return 0;
		}
	};

	class command_end: public command {
	public:
		command_end() {
			name = "end";
		}
		bool exec(parser& p) const override {
			if (!p.was_begin) {
				throw stack_emu::runtime_error("expected begin before end command");
			}
			p.was_begin = false;
			return 1;
		}
	};

	class command_label: public command {
	public:
		command_label() {
			name = "label";
		}
		bool check_name(std::string &name_) const override {
			return name_.size() > 0 && name_.back() == ':';
		}
	};

	class command_jmp: public command {
	public:
		command_jmp() {
			name = "jmp";
		}
		bool exec(parser& p) const override {
			GETLAB;
			p.current_command = stoi(lab);
			return 0;
		}

		bool compile(parser& p) const override {
			GETLAB;
			size_t label = p.get_label(lab);
			if (label == SIZE_MAX) {
				throw stack_emu::compile_error(name + ": no such label " + lab);
			}
			p.commands[p.current_command - 1] = to_string(label);
			return 0;
		}
	};

	class command_jeq: public command_jmp {
	public:
		command_jeq() {
			name = "jeq";
		}
		bool exec(parser& p) const override {
			GETLAB; GETAB;
			bool statement = a == b;
			if (statement) {
				p.current_command = stoi(lab);
			}
			return 0;
		}
	};

	class command_jne: public command_jmp {
	public:
		command_jne() {
			name = "jne";
		}
		bool exec(parser& p) const override {
			GETLAB; GETAB;
			bool statement = a != b;
			if (statement) {
				p.current_command = stoi(lab);
			}
			return 0;
		}
	};

	class command_ja: public command_jmp {
	public:
		command_ja() {
			name = "ja";
		}
		bool exec(parser& p) const override {
			GETLAB; GETAB;
			bool statement = a > b;
			if (statement) {
				p.current_command = stoi(lab);
			}
			return 0;
		}
	};

	class command_jae: public command_jmp {
	public:
		command_jae() {
			name = "jae";
		}
		bool exec(parser& p) const override {
			GETLAB; GETAB;
			bool statement = a >= b;
			if (statement) {
				p.current_command = stoi(lab);
			}
			return 0;
		}
	};

	class command_jb: public command_jmp {
	public:
		command_jb() {
			name = "jb";
		}
		bool exec(parser& p) const override {
			GETLAB; GETAB;
			bool statement = a < b;
			if (statement) {
				p.current_command = stoi(lab);
			}
			return 0;
		}
	};

	class command_jbe: public command_jmp {
	public:
		command_jbe() {
			name = "jbe";
		}
		bool exec(parser& p) const override {
			GETLAB; GETAB;
			bool statement = a <= b;
			if (statement) {
				p.current_command = stoi(lab);
			}
			return 0;
		}
	};

	class command_push: public command {
	public:
		command_push() {
			name = "push";
		}
		bool exec(parser& p) const override {
			GETVALUE;
			arithmetic::LongDouble v;
			CHECKV_RE;
			p.st.push(v);
			return 0;
		}

		bool compile(parser& p) const override {
			GETVALUE;
			arithmetic::LongDouble v;
			CHECKV;
			return 0;
		}
	};
	
	class command_pop: public command {
	public:
		command_pop() {
			name = "pop";
		}
		bool exec(parser& p) const override {
			if (p.st.size() == 0) {
				throw stack_emu::runtime_error("pop from empty stack");
			}
			p.st.pop();
			return 0;
		}
	};

	class command_pushr: public command {
	public:
		command_pushr() {
			name = "pushr";
		}
		bool exec(parser& p) const override {
			GETLAB;
			size_t r = stoi(lab);
			if (p.reg.size() < r + 1) {
				p.reg.resize(r + 1);
			}
			p.st.push(p.reg[r]);
			return 0;
		}

		bool compile(parser& p) const override {
			GETLAB;
			return 0;
		}
	};
	
	class command_popr: public command {
	public:
		command_popr() {
			name = "popr";
		}
		bool exec(parser& p) const override {
			GETLAB;
			size_t r = stoi(lab);
			if (p.reg.size() < r + 1) {
				p.reg.resize(r + 1);
			}
			if (p.st.size() == 0) {
				throw stack_emu::runtime_error("popr from empty stack");
			}
			p.reg[r] = p.st.top();
			p.st.pop();
			return 0;
		}

		bool compile(parser& p) const override {
			GETLAB;
			return 0;
		}
	};

	class command_add: public command {
	public:
		command_add() {
			name = "add";
		}
		bool exec(parser& p) const override {
			GETAB;
			p.st.push(a + b);
			return 0;
		}
	};

	class command_sub: public command {
	public:
		command_sub() {
			name = "sub";
		}
		bool exec(parser& p) const override {
			GETAB;
			p.st.push(a - b);
			return 0;
		}
	};

	class command_mul: public command {
	public:
		command_mul() {
			name = "mul";
		}
		bool exec(parser& p) const override {
			GETAB;
			p.st.push(a * b);
			return 0;
		}
	};

	class command_div: public command {
	public:
		command_div() {
			name = "div";
		}
		bool exec(parser& p) const override {
			GETAB;
			if (b.isZero()) {
				throw stack_emu::runtime_error(name + ": division by zero");
			}
			#ifdef USE_INT
			a.set_precision(1 + a.get_digits_size() / a.base + a.get_exponent());
			auto res = a / b;
			a.set_precision(INT_MAX);
			res.set_precision(INT_MAX);
			res.floor();
			#else
			auto res = a / b;
			#endif
			p.st.push(res);
			return 0;
		}
	};
	
	#ifndef USE_INT
	class command_sqrt: public command {
	public:
		command_sqrt() {
			name = "sqrt";
		}
		bool exec(parser& p) const override {
			if (p.st.size() < 1) {
				throw stack_emu::runtime_error(name + ": expected atleast 1 element in stack");
			}
			LongDouble a = p.st.top();
			if (a < 0) {
				throw stack_emu::runtime_error(name + ": sqrt negative value");
			}
			p.st.pop();
			a.sqrt();
			p.st.push(a);
			return 0;
		}
	};
	#endif

	class command_in: public command {
	public:
		command_in() {
			name = "in";
		}
		bool exec(parser& p) const override {
			string value;
			cin >> value;
			arithmetic::LongDouble v;
			CHECKV_RE;
			p.st.push(v);
			return 0;
		}
	};

	class command_out: public command {
	public:
		command_out() {
			name = "out";
		}
		bool exec(parser& p) const override {
			if (p.st.size() < 1) {
				throw stack_emu::runtime_error(name + ": expected atleast 1 element in stack");
			}
			cout << p.st.top() << endl;
			p.st.pop();
			return 0;
		}
	};

	class command_call: public command {
	public:
		command_call() {
			name = "call";
		}
		bool exec(parser& p) const override {
			GETLAB;
			#ifdef USE_CALL_STACK
			p.call_st.push(p.current_command);
			#else
			p.st.push(LongDouble((uint64_t) p.current_command));
			#endif
			p.current_command = (size_t) stoi(lab);
			return 0;
		}

		bool compile(parser& p) const override {
			GETLAB;
			size_t label = p.get_label(lab);
			if (label == SIZE_MAX) {
				throw stack_emu::compile_error(name + ": no such label " + lab);
			}
			p.commands[p.current_command - 1] = to_string(label);
			return 0;
		}

	};

	class command_ret: public command {
	public:
		command_ret() {
			name = "ret";
		}
		bool exec(parser& p) const override {
			#ifdef USE_CALL_STACK
			if (p.call_st.size() == 0) {
				throw stack_emu::runtime_error("trying to return, but call stack is empty");
			}
			p.current_command = p.call_st.top();
			p.call_st.pop();
			#else
			if (p.st.size() == 0) {
				throw stack_emu::runtime_error("trying to return, but stack is empty");
			}
			arithmetic::LongDouble top = p.st.top();
			p.current_command = (size_t) to_int(p.st.top());
			p.st.pop();
			#endif
			return 0;
		}
	};
}