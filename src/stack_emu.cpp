#include <stack.cpp>
#include <arithmetic.hpp>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <map>
#include <climits>
#include <cstring>
#include <vector>

using namespace stack_emu;
using namespace arithmetic;
using namespace std;

const char* DELIMITER = " ";
const char* XOR = "\0";

void compile(const char* filename) {
	ifstream stream;
	stream.open(filename);
	if (!stream.is_open()) {
		cout << "Cannot open file " << filename << endl;
		return;
	}
	
	string inp;
	LongDouble::default_precision = INT_MAX;
	cout << setprecision(INT_MAX);
	char **commands = (char**) malloc(0);
	size_t commands_size = 0;
	map<string, size_t> labels;
	bool was_label = false;
	while (stream >> inp) { // compile
		cout << "'" << inp << "'" << endl;
		commands_size++;
		commands = (char**) realloc(commands, commands_size * sizeof(char*));
		char* command = (char*) malloc(inp.size() * sizeof(char));
		memcpy(command, inp.data(), inp.size());
		commands[commands_size - 1] = command;
		if (was_label) {
			if (labels.count(inp)) {
				throw runtime_error("LABEL: Expected unique name");
			}
			labels[inp] = commands_size;
			was_label = false;
		} else if (inp == "LABEL") {
			was_label = true;
		}
	}
	if (was_label) {
		throw runtime_error("LABEL: Expected input");
	}
	stream.close();
	ofstream out(string(filename) + ".out", ios::binary);
	if (!out.is_open()) {
		cout << "Cannot write file " << filename << ".out" << endl;
		return;
	}
	for (size_t i = 0; i < commands_size; i++) {
		size_t len = strlen(commands[i]);
		for (size_t j = 0; j < len; j++) {
			commands[i][j] ^= XOR[0];
		}
		out.write(commands[i], strlen(commands[i]) * sizeof(char));
		if (i + 1 < commands_size) {
			char what_write[1];
			what_write[0] = (char) (DELIMITER[0] ^ XOR[0]);
			out.write(what_write, strlen(DELIMITER) * sizeof(char));
		}
	}
}

void emulate(const char* filename) {
	// run
	// read bin

	char **commands = (char**) malloc(sizeof(char*));
	size_t commands_size = 1;
	commands[commands_size - 1] = (char*) malloc(0);

	ifstream stream(filename, ios::binary);
	if (!stream.is_open()) {
		cout << "Cannot open file " << filename << endl;
		return;
	}

	char read[1];
	size_t current_size = 0;

	while (stream.read(read, 1), stream) {
		read[0] ^= XOR[0];
		if (read[0] == DELIMITER[0]) {
			commands_size++;
			commands = (char**) realloc(commands, commands_size * sizeof(char*));
			commands[commands_size - 1] = (char*) malloc(0);
			current_size = 0;
		} else {
			current_size++;
			commands[commands_size - 1] = (char*) realloc(commands[commands_size - 1], current_size * sizeof(char));
			commands[commands_size - 1][current_size - 1] = read[0];
		}
	}
	stream.close();
	if (current_size == 0 && commands_size == 1) {
		commands_size--;
	}

	string inp;
	map<string, size_t> labels;
	for (size_t i = 0; i < commands_size; i++) {
		if (commands[i] == string("LABEL")) {
			if (i == commands_size - 1) {
				throw runtime_error("LABEL: Expected input");
			}
			labels[commands[i + 1]] = i + 2;
		}
	}
	LongDouble::default_precision = INT_MAX;
	cout << setprecision(INT_MAX);

	stack<LongDouble> st;
	bool was_begin = false;
	const size_t REG_SIZE = 32;
	LongDouble reg[REG_SIZE] = {};
	
	size_t current_command = 0;
	while (current_command < commands_size) {
		inp = commands[current_command++];
		if (inp == "BEGIN") {
			if (was_begin) {
				throw runtime_error("Double BEGIN command");
			}
			was_begin = true;
		} else if (inp == "END") {
			if (!was_begin) {
				throw runtime_error("Expected BEGIN before END command");
			}
			was_begin = false;
			break;
		} else if (was_begin == false) {
			throw runtime_error("Expected BEGIN before " + inp);
		} else if (inp == "LABEL") {
			if (current_command == commands_size) {
				throw runtime_error("LABEL: Expected input");
			}
			inp = commands[current_command++];
		} else if (inp == "JMP") {
			if (current_command == commands_size) {
				throw runtime_error("JMP: Expected input");
			}
			inp = commands[current_command++];
			if (labels.count(inp) == 0) {
				throw runtime_error("JMP: No such label: " + inp);
			}
			current_command = labels[inp];
		} else if (inp == "JEQ") {
			if (current_command == commands_size) {
				throw runtime_error(inp + ": Expected input");
			}
			inp = commands[current_command++];
			if (labels.count(inp) == 0) {
				throw runtime_error(inp + ": No such label: " + inp);
			}
			if (st.size() < 2) {
				throw runtime_error(inp + ": Expected atleast 2 elements in stack");
			}
			auto a = st.top();
			st.pop();
			auto b = st.top();
			st.push(a);
			bool statement = a == b;
			if (statement) {
				current_command = labels[inp];
			}
		} else if (inp == "JNE") {
			if (current_command == commands_size) {
				throw runtime_error(inp + ": Expected input");
			}
			inp = commands[current_command++];
			if (labels.count(inp) == 0) {
				throw runtime_error(inp + ": No such label: " + inp);
			}
			if (st.size() < 2) {
				throw runtime_error(inp + ": Expected atleast 2 elements in stack");
			}
			auto a = st.top();
			st.pop();
			auto b = st.top();
			st.push(a);
			bool statement = a != b;
			if (statement) {
				current_command = labels[inp];
			}
		} else if (inp == "JA") {
			if (current_command == commands_size) {
				throw runtime_error(inp + ": Expected input");
			}
			inp = commands[current_command++];
			if (labels.count(inp) == 0) {
				throw runtime_error(inp + ": No such label: " + inp);
			}
			if (st.size() < 2) {
				throw runtime_error(inp + ": Expected atleast 2 elements in stack");
			}
			auto a = st.top();
			st.pop();
			auto b = st.top();
			st.push(a);
			bool statement = a > b;
			if (statement) {
				current_command = labels[inp];
			}
		} else if (inp == "JAE") {
			if (current_command == commands_size) {
				throw runtime_error(inp + ": Expected input");
			}
			inp = commands[current_command++];
			if (labels.count(inp) == 0) {
				throw runtime_error(inp + ": No such label: " + inp);
			}
			if (st.size() < 2) {
				throw runtime_error(inp + ": Expected atleast 2 elements in stack");
			}
			auto a = st.top();
			st.pop();
			auto b = st.top();
			st.push(a);
			bool statement = a >= b;
			if (statement) {
				current_command = labels[inp];
			}
		} else if (inp == "JB") {
			if (current_command == commands_size) {
				throw runtime_error(inp + ": Expected input");
			}
			inp = commands[current_command++];
			if (labels.count(inp) == 0) {
				throw runtime_error(inp + ": No such label: " + inp);
			}
			if (st.size() < 2) {
				throw runtime_error(inp + ": Expected atleast 2 elements in stack");
			}
			auto a = st.top();
			st.pop();
			auto b = st.top();
			st.push(a);
			bool statement = a < b;
			if (statement) {
				current_command = labels[inp];
			}
		} else if (inp == "JBE") {
			if (current_command == commands_size) {
				throw runtime_error(inp + ": Expected input");
			}
			inp = commands[current_command++];
			if (labels.count(inp) == 0) {
				throw runtime_error(inp + ": No such label: " + inp);
			}
			if (st.size() < 2) {
				throw runtime_error(inp + ": Expected atleast 2 elements in stack");
			}
			auto a = st.top();
			st.pop();
			auto b = st.top();
			st.push(a);
			bool statement = a <= b;
			if (statement) {
				current_command = labels[inp];
			}
		} else if (inp == "PUSH") {
			if (current_command == commands_size) {
				throw runtime_error("PUSH: Expected input");
			}
			string value = commands[current_command++];
			LongDouble v;
			bool error = false;
			try {
				v = value;
			} catch (const InitStringError &e) {
				error = true;
			}
			if (error || !v.isInt()) {
				throw runtime_error("PUSH: Wrong value format: " + value + ", expected: [-+][0-9]");
			}
			st.push(v);
		} else if (inp == "POP") {
			if (st.size() == 0) {
				throw runtime_error("POP from empty stack");
			}
			st.pop();
		} else if (inp == "PUSHR") {
			if (current_command == commands_size) {
				throw runtime_error("PUSHR: Expected input");
			}
			size_t r = atoi(commands[current_command++]);
			if (r >= REG_SIZE) {
				throw runtime_error("PUSHR: Register must be in [0, " + to_string(REG_SIZE) + "]");
			}
			st.push(reg[r]);
		} else if (inp == "POPR") {
			if (current_command == commands_size) {
				throw runtime_error("POPR: Expected input");
			}
			size_t r = atoi(commands[current_command++]);
			if (r >= REG_SIZE) {
				throw runtime_error("PUSHR: Register must be in [0, " + to_string(REG_SIZE) + "]");
			}
			if (st.size() == 0) {
				throw runtime_error("POPR from empty stack");
			}
			reg[r] = st.top();
			st.pop();
		} else if (inp == "ADD") {
			if (st.size() < 2) {
				throw runtime_error("ADD: Expected atleast 2 elements in stack");
			}
			LongDouble b, a;
			b = st.top();
			st.pop();
			a = st.top();
			st.pop();
			st.push(a + b);
		} else if (inp == "SUB") {
			if (st.size() < 2) {
				throw runtime_error("SUB: Expected atleast 2 elements in stack");
			}
			LongDouble b, a;
			b = st.top();
			st.pop();
			a = st.top();
			st.pop();
			st.push(a - b);
		} else if (inp == "MUL") {
			if (st.size() < 2) {
				throw runtime_error("MUL: Expected atleast 2 elements in stack");
			}
			LongDouble b, a;
			b = st.top();
			st.pop();
			a = st.top();
			st.pop();
			st.push(a * b);
		} else if (inp == "DIV") {
			if (st.size() < 2) {
				throw runtime_error("DIV: Expected atleast 2 elements in stack");
			}
			LongDouble b, a;
			b = st.top();
			st.pop();
			a = st.top();
			st.pop();
			if (b.isZero()) {
				throw runtime_error("DIV: Division by zero");
			}
			a.set_precision(a.get_digits_size() + a.get_exponent());
			auto res = a / b;
			a.set_precision(INT_MAX);
			res.set_precision(INT_MAX);
			res.floor();
			st.push(res);
		} else if (inp == "OUT") {
			if (st.size() < 1) {
				throw runtime_error("OUT: Expected atleast 1 element in stack");
			}
			cout << st.top() << endl;
			st.pop();
		} else if (inp == "IN") {
			string value;
			cin >> value;
			LongDouble v;
			bool error = false;
			try {
				v = value;
			} catch (const InitStringError &e) {
				error = true;
			}
			if (error || !v.isInt()) {
				throw runtime_error("IN: Wrong value format: " + value + ", expected: [-+][0-9]");
			}
			st.push(v);
		} else {
			throw runtime_error("Unexpected command " + inp);
		}
	}
	for (size_t i = 0; i < commands_size; i++) {
		free(commands[i]);
	}
	free(commands);
	if (was_begin) {
		throw runtime_error("END command expected");
	}
}

int main(int argc, char* argv[]) {

	if (argc != 3 || !(argv[1] == string("compile") || argv[1] == string("emulate"))) {
		cout << "Usage: {executable} {compile|emulate} {path_to_file}" << endl;
		return 0;
	}
	if (argv[1] == string("compile")) {
		compile(argv[2]);
	} else {
		emulate(argv[2]);
	}
 
}