#include <stack.cpp>
#include <arithmetic.hpp>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

using namespace stack_emu;
using namespace arithmetic;
using namespace std;

int main(int argc, char* argv[]) {

	if (argc != 2) {
		cout << "Usage: {executable} {path_to_file}" << endl;
		return 0;
	}
	ifstream sin;
	sin.open(argv[1]);
	if (!sin.is_open()) {
		cout << "Cannot open file " << argv[1] << endl;
		return 0;
	}
	cout << "Reading file " << argv[1] << endl;
	string inp;
	bool was_begin = false;
	LongDouble::default_precision = INT_MAX;
	cout << setprecision(INT_MAX);
	stack<LongDouble> st;
	const size_t REG_SIZE = 32;
	LongDouble reg[REG_SIZE] = {};
	while (sin >> inp) {
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
		} else if (inp == "PUSH") {
			string value;
			sin >> value;
			LongDouble v;
			bool error = false;
			try {
				v = value;
			} catch (InitStringError e) {
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
			size_t r;
			sin >> r;
			if (r < 0 || r >= REG_SIZE) {
				throw runtime_error("PUSHR: Register must be in [0, " + to_string(REG_SIZE) + "]");
			}
			st.push(reg[r]);
		} else if (inp == "POPR") {
			size_t r;
			sin >> r;
			if (r < 0 || r >= REG_SIZE) {
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
			a.precision = a.digits_size + a.exponent;
			auto res = a / b;
			a.precision = INT_MAX;
			res.precision = INT_MAX;
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
			} catch (InitStringError e) {
				error = true;
			}
			if (error || !v.isInt()) {
				throw runtime_error("IN: Wrong value format: " + value + ", expected: [-+][0-9]");
			}
			st.push(v);
		} else {
			runtime_error("Unexpected command " + inp);
		}
	}
	sin.close();
	if (was_begin) {
		throw runtime_error("END command expected");
	}
	cout << "Finished" << endl;

 
}