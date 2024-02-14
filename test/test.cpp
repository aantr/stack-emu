#include <iostream>
#include <test_system.hpp>
#include <stack.cpp>

using namespace std;
using namespace stack_emu;
using namespace test_system;

// test system init && add define
TestSystem ts;
#undef TEST_SYSTEM
#define TEST_SYSTEM ts

void add_tests() {

TEST_ (Init)

	stack<int> s; // empty
	stack<int> s1(5); // 5 zeroes
	stack<int> s2(5, 5); // 5 fives
	for (int i = 0; i < 5; i++) {
		s.push(i + 1);
	}
	ASSERT(s.size() == 5);
	ASSERT(s1.size() == 5);
	ASSERT(s1.top() == 0);
	ASSERT(s2.top() == 5);


_TEST

TEST_ (PushPop)

	stack<int> s; // empty
	stack<int> s1(5); // 5 zeroes
	stack<int> s2(5, 5); // 5 fives
	for (int i = 0; i < 5; i++) {
		s.push(i + 1);
	}
	for (int i = 0; i < 5; i++) {
		ASSERT(s.top() == 5 - i);
		s.pop();
	}
	bool was = false;
	try {
		s.pop();
	} catch (exception e) {
		was= true;
	}
	ASSERT(was);
	ASSERT(s.empty());


_TEST

TEST_ (Equality)

	stack<int> s; // empty
	stack<int> s1(5); // 5 zeroes
	stack<int> s2(5, 5); // 5 fives
	ASSERT(s != s2);
	for (int i = 0; i < 5; i++) {
		s.push(0);
	}
	ASSERT(s == s1);


_TEST


TEST_ (FiveRule)

	stack<int> s; // empty
	stack<int> s1(5); // 5 zeroes
	stack<int> s2(5, 5); // 5 fives
	
	stack<int> s3{std::move(s2)};
	ASSERT(s3 == s2);

	s1 = s3;
	ASSERT(s1 == s2); // copy
	ASSERT(s2 != s3); // copy effect


_TEST


}


int main() {
	add_tests();
	ts.run_testing(std::cout);
	return 0;
}