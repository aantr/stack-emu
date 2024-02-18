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

using namespace stack_emu;
using namespace arithmetic;
using namespace std;

// #define DEBUG
// #define USE_INT		
#define PRECISION 16

const string DELIMITER = "#";

class compile_error: public exception {
public:
	string msg;
	compile_error(const string msg_): msg(msg_) {

	}
    const char * what() const noexcept override {
        return msg.c_str();
    }
};

bool compile(const char* filename, const char* dest) {

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
				throw compile_error("label declare should be unique " + commands[i].substr(0, commands[i].size() - 1));
			}
			labels.push_back({commands[i].substr(0, commands[i].size() - 1), i + 1});
			commands[i] = to_string(i + 1) + ":";
		}
	}
	for (size_t i = 0; i < commands.size(); i++) {
		if (commands[i] == "pushr" || commands[i] == "popr") {
			if (i == commands.size() - 1) {
				throw compile_error(commands[i] + ": expected input");
			}
			if (get_register(commands[i + 1]) != SIZE_MAX) {
				commands[i + 1] = to_string(get_register(commands[i + 1]));
			} else {
				registers.push_back({commands[i + 1], registers.size()});
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
		if (inp == "begin") {

		} else if (inp == "end") {

		} else if (inp.back() == ':') {

		} else if (inp == "jmp" || inp == "jeq" || inp == "jne" || inp == "ja" || inp == "jb" || inp == "jae" || inp == "jbe") {
			if (current_command == commands.size()) {
				throw compile_error(inp + ": expected input");
			}
			string lab = commands[current_command++];
			if (get_label(lab) == SIZE_MAX) {
				throw compile_error(inp + ": no such label " + lab);
			}
			commands[current_command - 1] = to_string(get_label(lab));
		} else if (inp == "push") {
			if (current_command == commands.size()) {
				throw compile_error(inp + ": expected input");
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
				throw compile_error(inp + ": wrong value format: " + value + ", expected: [-+][0-9]");
			}
			#else 
			if (error) {
				throw compile_error(inp + ": wrong value format: " + value + ", expected: [-+][0-9][.][0-9]");
			}
			#endif
		} else if (inp == "pop") {

		} else if (inp == "pushr") {
			if (current_command == commands.size()) {
				throw compile_error(inp + ": expected input");
			}
			string reg = commands[current_command++];
		} else if (inp == "popr") {
			if (current_command == commands.size()) {
				throw compile_error(inp + ": expected input");
			}
			string reg = commands[current_command++];
		} else if (inp == "add") {

		} else if (inp == "sub") {

		} else if (inp == "mul") {

		} else if (inp == "div") {

		}
		#ifndef USE_INT
		else if (inp == "sqrt") {

		}
		#endif
		else if (inp == "out") {

		} else if (inp == "in") {

		} else if (inp == "ret") {

		} else if (inp == "call") {
			if (current_command == commands.size()) {
				throw compile_error(inp + ": expected input");
			}
			string lab = commands[current_command++];
			if (get_label(lab) == SIZE_MAX) {
				throw compile_error(inp + ": no such label " + lab);
			}
			commands[current_command - 1] = to_string(get_label(lab));
		} else {
			throw compile_error("unexpected command " + inp);
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

bool emulate(const char* filename) {
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

	string inp;
	
	#ifdef USE_INT
	LongDouble::default_precision = INT_MAX;
	cout << setprecision(INT_MAX);
	#else
	LongDouble::default_precision = PRECISION / LongDouble::base_exp + 1;
	cout << setprecision(PRECISION);
	#endif

	stack_emu::stack<LongDouble> st;
	stack_emu::stack<size_t> call_st;
	stack_emu::vector<LongDouble> reg;
	size_t current_command = 0;
	bool was_begin = false;

	while (current_command < commands.size()) {
		cout << "reg: ";
		for (auto i : reg) {
			cout << i << " ";
		}
		cout << endl;
		cout << "reg1: ";
		for (size_t i = 0; i < reg.size(); i++) {
			cout << reg[i] << " ";
		}
		cout << endl;
		cout << "reg2: ";
		for (auto i = reg.rbegin(); i != reg.rend(); i++) {
			cout << *i << " ";
		}
		cout << endl;
		inp = commands[current_command++];
		std::transform(inp.begin(), inp.end(), inp.begin(),
    			[](unsigned char c){ return std::tolower(c); });
		if (inp == "begin") {
			if (was_begin) {
				throw runtime_error("double begin command");
			}
			was_begin = true;
		} else if (inp == "end") {
			if (!was_begin) {
				throw runtime_error("expected begin before end command");
			}
			was_begin = false;
			break;
		} else if (was_begin == false) {
			// if begin is the first command ?
			// throw runtime_error("expected begin before " + inp);
		} else if (inp.back() == ':') {

		} else if (inp == "jmp") {
			if (current_command == commands.size()) {
				throw runtime_error(inp + ": expected input");
			}
			string lab = commands[current_command++];
			current_command = stoi(lab);
		} else if (inp == "jeq") {
			if (current_command == commands.size()) {
				throw runtime_error(inp + ": expected input");
			}
			string lab = commands[current_command++];
			if (st.size() < 2) {
				throw runtime_error(inp + ": expected atleast 2 elements in stack");
			}
			auto b = st.top();
			st.pop();
			auto a = st.top();
			st.pop();
			bool statement = a == b;
			if (statement) {
				current_command = stoi(lab);
			}
		} else if (inp == "jne") {
			if (current_command == commands.size()) {
				throw runtime_error(inp + ": expected input");
			}
			string lab = commands[current_command++];
			if (st.size() < 2) {
				throw runtime_error(inp + ": expected atleast 2 elements in stack");
			}
			auto b = st.top();
			st.pop();
			auto a = st.top();
			st.pop();
			bool statement = a != b;
			if (statement) {
				current_command = stoi(lab);
			}
		} else if (inp == "ja") {
			if (current_command == commands.size()) {
				throw runtime_error(inp + ": expected input");
			}
			string lab = commands[current_command++];
			if (st.size() < 2) {
				throw runtime_error(inp + ": expected atleast 2 elements in stack");
			}
			auto b = st.top();
			st.pop();
			auto a = st.top();
			st.pop();
			bool statement = a > b;
			if (statement) {
				current_command = stoi(lab);
			}
		} else if (inp == "jae") {
			if (current_command == commands.size()) {
				throw runtime_error(inp + ": expected input");
			}
			string lab = commands[current_command++];
			if (st.size() < 2) {
				throw runtime_error(inp + ": expected atleast 2 elements in stack");
			}
			auto b = st.top();
			st.pop();
			auto a = st.top();
			st.pop();
			bool statement = a >= b;
			if (statement) {
				current_command = stoi(lab);
			}
		} else if (inp == "jb") {
			if (current_command == commands.size()) {
				throw runtime_error(inp + ": expected input");
			}
			string lab = commands[current_command++];
			if (st.size() < 2) {
				throw runtime_error(inp + ": expected atleast 2 elements in stack");
			}
			auto b = st.top();
			st.pop();
			auto a = st.top();
			st.pop();
			bool statement = a < b;
			if (statement) {
				current_command = stoi(lab);
			}
		} else if (inp == "jbe") {
			if (current_command == commands.size()) {
				throw runtime_error(inp + ": expected input");
			}
			string lab = commands[current_command++];
			if (st.size() < 2) {
				throw runtime_error(inp + ": expected atleast 2 elements in stack");
			}
			auto b = st.top();
			st.pop();
			auto a = st.top();
			st.pop();
			bool statement = a <= b;
			if (statement) {
				current_command = stoi(lab);
			}
		} else if (inp == "push") {
			if (current_command == commands.size()) {
				throw runtime_error(inp + ": expected input");
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
				throw compile_error(inp + ": wrong value format: " + value + ", expected: [-+][0-9]");
			}
			#else 
			if (error) {
				throw compile_error(inp + ": wrong value format: " + value + ", expected: [-+][0-9][.][0-9]");
			}
			#endif
			st.push(v);
		} else if (inp == "pop") {
			if (st.size() == 0) {
				throw runtime_error("pop from empty stack");
			}
			st.pop();
		} else if (inp == "pushr") {
			if (current_command == commands.size()) {
				throw runtime_error(inp + ": expected input");
			}
			size_t r = stoi(commands[current_command++]);
			if (reg.size() < r + 1) {
				reg.resize(r + 1);
			}
			st.push(reg[r]);
		} else if (inp == "popr") {
			if (current_command == commands.size()) {
				throw runtime_error(inp + ": expected input");
			}
			size_t r = stoi(commands[current_command++]);
			if (reg.size() < r + 1) {
				reg.resize(r + 1);
			}
			if (st.size() == 0) {
				throw runtime_error("popr from empty stack");
			}
			reg[r] = st.top();
			st.pop();
		} else if (inp == "add") {
			if (st.size() < 2) {
				throw runtime_error(inp + ": expected atleast 2 elements in stack");
			}
			LongDouble b, a;
			b = st.top();
			st.pop();
			a = st.top();
			st.pop();
			st.push(a + b);
			#ifdef DEBUG
			cout << inp << " " << a << " " << b << " res: " << st.top() << endl;
			#endif
		} else if (inp == "sub") {
			if (st.size() < 2) {
				throw runtime_error(inp + ": expected atleast 2 elements in stack");
			}
			LongDouble b, a;
			b = st.top();
			st.pop();
			a = st.top();
			st.pop();
			st.push(a - b);
			#ifdef DEBUG
			cout << inp << " " << a << " " << b << " res: " << st.top() << endl;
			#endif
		} else if (inp == "mul") {
			if (st.size() < 2) {
				throw runtime_error(inp + ": expected atleast 2 elements in stack");
			}
			LongDouble b, a;
			b = st.top();
			st.pop();
			a = st.top();
			st.pop();
			st.push(a * b);
			#ifdef DEBUG
			cout << inp << " " << a << " " << b << " res: " << st.top() << endl;
			#endif
		} else if (inp == "div") {
			if (st.size() < 2) {
				throw runtime_error(inp + ": expected atleast 2 elements in stack");
			}
			LongDouble b, a;
			b = st.top();
			st.pop();
			a = st.top();
			st.pop();
			if (b.isZero()) {
				throw runtime_error(inp + ": division by zero");
			}
			#ifdef USE_INT
			a.set_precision(a.get_digits_size() + a.get_exponent());
			auto res = a / b;
			a.set_precision(INT_MAX);
			res.set_precision(INT_MAX);
			res.floor();
			#else
			auto res = a / b;
			#endif
			st.push(res);
			#ifdef DEBUG
			cout << inp << " " << a << " " << b << " res: " << st.top() << endl;
			#endif
		}
		#ifndef USE_INT
		else if (inp == "sqrt") {
			if (st.size() < 1) {
				throw runtime_error(inp + ": expected atleast 1 element in stack");
			}
			LongDouble a;
			a = st.top();
			if (a < 0) {
				throw runtime_error(inp + ": sqrt negative value");
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
				throw runtime_error(inp + ": expected atleast 1 element in stack");
			}
			cout << st.top() << endl;
			st.pop();
		} else if (inp == "in") {
			string value;
			cin >> value;
			LongDouble v;
			bool error = false;
			try {
				v = value;
			} catch (const InitStringError &e) {
				error = true;
			}
			#ifdef USE_INT
			if (error || !v.isInt()) {
				throw compile_error(inp + ": wrong value format: " + value + ", expected: [-+][0-9]");
			}
			#else 
			if (error) {
				throw compile_error(inp + ": wrong value format: " + value + ", expected: [-+][0-9][.][0-9]");
			}
			#endif
			st.push(v);
		} else if (inp == "call") {
			if (current_command == commands.size()) {
				throw runtime_error(inp + ": expected input");
			}
			string lab = commands[current_command++];
			call_st.push(current_command);
			current_command = (size_t) stoi(lab);
		} else if (inp == "ret") {
			if (call_st.size() == 0) {
				throw runtime_error("trying to return, but call stack is empty");
			}
			current_command = call_st.top();
			call_st.pop();
		} else {
			throw runtime_error("unexpected command " + inp);
		}
	}
	if (was_begin) {
		throw runtime_error("end command expected");
	}
	return 1;
}

int main(int argc, char* argv[]) {

	if (argc != 3 || !(argv[1] == string("compile") || argv[1] == string("emulate") || argv[1] == string("compulate"))) {
		cout << "Usage: {executable} {compile|emulate|compulate (both)} {path_to_file}" << endl;
	} else if (argv[1] == string("compile")) {
		string dest = string(argv[2]) + ".out";
		if (compile(argv[2], dest.c_str())) {
			cout << dest << endl;
		}
	} else if (argv[1] == string("emulate")){
		emulate(argv[2]);
	} else {
		string dest = string(argv[2]) + ".out";
		if (compile(argv[2], dest.c_str())) {
			cout << dest << endl;
		}
		emulate(dest.c_str());
	}
	return 0;

}