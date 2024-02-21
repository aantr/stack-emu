#include <stack.hpp>
#include <arithmetic.hpp>
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

using namespace stack_emu;
using namespace arithmetic;
using std::endl;
using std::string;
using std::cin;
using std::cout;

#define GETLAB \
if (current_command == commands.size()) { \
	throw stack_emu::runtime_error(inp + ": expected input"); \
} \
std::string lab = commands[current_command++];

#define GETVALUE \
if (current_command == commands.size()) { \
	throw stack_emu::runtime_error(inp + ": expected input"); \
} \
std::string value = commands[current_command++];

#define GETAB \
if (st.size() < 2) { \
	throw stack_emu::runtime_error(inp + ": expected atleast 2 elements in stack"); \
} \
LongDouble b, a; \
b = st.top(); \
st.pop(); \
a = st.top(); \
st.pop();

#ifdef USE_INT
#define CHECKV \
bool error = false; \
try { \
	v = value; \
} catch (const InitStringError &e) { \
	error = true; \
} \
if (error) { \
	throw compile_error(inp + ": wrong value format: " + value + ", expected: [-+]?[0-9]*[.]?[0-9]*"); \
}
#else
#define CHECKV \
bool error = false; \
try { \
	v = value; \
} catch (const InitStringError &e) { \
	error = true; \
} \
if (error) { \
	throw compile_error(inp + ": wrong value format: " + value + ", expected: [-+]?[0-9]*[.]?[0-9]*"); \
}
#endif

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

bool stack_emu::emulate(const char* filename) {
	// run
	// read bin

	stack_emu::vector<string> commands;
	commands.push_back("");
	ifstream stream(filename, ios::binary);
	if (!stream.is_open()) {
		cout << "Cannot open file " << filename << endl;
		return 0;
	}

	char read[1];
	while (stream.read(read, 1), stream) {
		if (read[0] == DELIMITER[0]) {
			commands.push_back("");
		} else {
			commands.back() += read[0];
		}
	}
	stream.close();
	if (commands.size() == 1 && commands.back().size() == 0) {
		commands.pop_back();
	}

	// preset
	string inp;
	#ifdef USE_INT
	LongDouble::default_precision = INT_MAX;
	cout << setprecision(INT_MAX);
	#else
	LongDouble::default_precision = PRECISION / LongDouble::base_exp + 1;
	cout << setprecision(PRECISION);
	#endif

	stack_emu::stack<LongDouble> st;
	#ifdef USE_CALL_STACK
	stack_emu::stack<size_t> call_st;
	#endif
	stack_emu::vector<LongDouble> reg;
	size_t current_command = 0;
	bool was_begin = false;

	// emulaute
	while (current_command < commands.size()) {
		inp = commands[current_command++];
		std::transform(inp.begin(), inp.end(), inp.begin(),
    			[](unsigned char c){ return std::tolower(c); });
		if (inp == "begin") {
			if (was_begin) {
				throw stack_emu::runtime_error("double begin command");
			}
			was_begin = true;
		} else if (inp == "end") {
			if (!was_begin) {
				throw stack_emu::runtime_error("expected begin before end command");
			}
			was_begin = false;
			break;
		} else if (was_begin == false) {
			// if begin is the first command ?
			// throw stack_emu::runtime_error("expected begin before " + inp);
		} else if (inp.back() == ':') {

		} else if (inp == "jmp") {
			GETLAB;
			current_command = stoi(lab);
		} else if (inp == "jeq") {
			GETLAB; GETAB;
			bool statement = a == b;
			if (statement) {
				current_command = stoi(lab);
			}
		} else if (inp == "jne") {
			GETLAB; GETAB;
			bool statement = a != b;
			if (statement) {
				current_command = stoi(lab);
			}
		} else if (inp == "ja") {
			GETLAB; GETAB;
			bool statement = a > b;
			if (statement) {
				current_command = stoi(lab);
			}
		} else if (inp == "jae") {
			GETLAB; GETAB;
			bool statement = a >= b;
			if (statement) {
				current_command = stoi(lab);
			}
		} else if (inp == "jb") {
			GETLAB; GETAB;
			bool statement = a < b;
			if (statement) {
				current_command = stoi(lab);
			}
		} else if (inp == "jbe") {
			GETLAB; GETAB;
			bool statement = a <= b;
			if (statement) {
				current_command = stoi(lab);
			}
		} else if (inp == "push") {
			GETVALUE;
			LongDouble v;
			CHECKV;
			st.push(v);
		} else if (inp == "pop") {
			if (st.size() == 0) {
				throw stack_emu::runtime_error("pop from empty stack");
			}
			st.pop();
		} else if (inp == "pushr") {
			GETLAB;
			size_t r = stoi(lab);
			if (reg.size() < r + 1) {
				reg.resize(r + 1);
			}
			st.push(reg[r]);
		} else if (inp == "popr") {
			GETLAB;
			size_t r = stoi(lab);
			if (reg.size() < r + 1) {
				reg.resize(r + 1);
			}
			if (st.size() == 0) {
				throw stack_emu::runtime_error("popr from empty stack");
			}
			reg[r] = st.top();
			st.pop();
		} else if (inp == "add") {
			GETAB;
			st.push(a + b);
		} else if (inp == "sub") {
			GETAB;
			st.push(a - b);
		} else if (inp == "mul") {
			GETAB;
			st.push(a * b);
		} else if (inp == "div") {
			GETAB;
			if (b.isZero()) {
				throw stack_emu::runtime_error(inp + ": division by zero");
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
			st.push(res);
		}
		#ifndef USE_INT
		else if (inp == "sqrt") {
			if (st.size() < 1) {
				throw stack_emu::runtime_error(inp + ": expected atleast 1 element in stack");
			}
			LongDouble a = st.top();
			if (a < 0) {
				throw stack_emu::runtime_error(inp + ": sqrt negative value");
			}
			st.pop();
			a.sqrt();
			st.push(a);
			#ifdef DEBUG
			cout << inp << " " << a << " res: " << st.top() << endl;
			#endif
		}
		#endif
		else if (inp == "out") {
			if (st.size() < 1) {
				throw stack_emu::runtime_error(inp + ": expected atleast 1 element in stack");
			}
			cout << st.top() << endl;
			st.pop();
		} else if (inp == "in") {
			string value;
			cin >> value;
			LongDouble v;
			CHECKV;
			st.push(v);
		} else if (inp == "call") {
			GETLAB;
			#ifdef USE_CALL_STACK
			call_st.push(current_command);
			#else
			st.push(LongDouble((uint64_t) current_command));
			#endif
			current_command = (size_t) stoi(lab);
		} else if (inp == "ret") {
			#ifdef USE_CALL_STACK
			if (call_st.size() == 0) {
				throw stack_emu::runtime_error("trying to return, but call stack is empty");
			}
			current_command = call_st.top();
			call_st.pop();
			#else
			if (st.size() == 0) {
				throw stack_emu::runtime_error("trying to return, but stack is empty");
			}
			LongDouble top = st.top();
			current_command = (size_t) to_int(st.top());
			st.pop();
			#endif
		} else {
			throw stack_emu::runtime_error("unexpected command " + inp);
		}
	}
	if (was_begin) {
		throw stack_emu::runtime_error("end command expected");
	}
	return 1;
}
