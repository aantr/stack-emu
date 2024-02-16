#include <iostream>
#include <test_system.hpp>
#include <stack.cpp>
#include <vector.cpp>
#include <deque.cpp>

// using namespace std;
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
	// for (int i = 0; i < 5; i++) {
	// 	s1.push(0);
	// }
	stack<int> s2(5, 5); // 5 fives
	// for (int i = 0; i < 5; i++) {
	// 	s2.push(5);
	// }

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
	} catch (const exception &e) {
		was = true;
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
	for (int i = 0; i < 5; i++) {
		s.pop();
		s1.pop();
		s2.pop();	
	} cout << endl;

	ASSERT(s == s1);



_TEST


TEST_ (FiveRule)

	stack<int> s; // empty
	stack<int> s1(5); // 5 zeroes
	stack<int> s2(5, 5); // 5 fives
	stack<int> s3 = s2; // copy
	ASSERT(s3 == s2);

	stack<int> s4{std::move(s3)};
	ASSERT(s4 == s2); // move
	ASSERT(s2 != s3); // move effect

	s3 = s2; // copy
	ASSERT(s3 == s2);

_TEST

TEST_ (VectorFiveRule)

	stack_emu::vector<int> s; // empty
	stack_emu::vector<int> s1(5); // 5 zeroes
	stack_emu::vector<int> s2(5, 5); // 5 fives
	for (int i = 0; i < 5; i++) {
		s2[i]--;
		ASSERT(s2[i] == 4);
	}
	stack_emu::vector<int> s3 = s2; // copy

	ASSERT(s3 == s2);

	stack_emu::vector<int> s4{std::move(s3)};
	ASSERT(s4 == s2); // move
	ASSERT(s2 != s3); // move effect

	s3 = s2; // copy
	ASSERT(s3 == s2);

_TEST


TEST_ (deque)

	deque<int> s; // empty
	deque<int> s1(5); // 5 zeroes
	deque<int> s2(5, 5); // 5 fives
	for (int i = 0; i < 5; i++) {
		s2[i]--;
		ASSERT(s2[i] == 4);
	}
	deque<int> s3 = s2; // copy

	ASSERT(s3 == s2);

	deque<int> s4{std::move(s3)};
	ASSERT(s4 == s2); // move
	ASSERT(s2 != s3); // move effect

	s3 = s2; // copy
	ASSERT(s3 == s2);

	s1.resize(0);
	ASSERT(s1.size() == 0);
	
	for (int i = 0; i < 1000000; i++) {
		s1.push_back(1);
		s1.push_front(2);
	}
	for (int i = 0; i < 500000; i++) {
		s1.pop_back();
		s1.pop_front();
	}




_TEST

}



int main() {
	add_tests();
	ts.run_testing(std::cout);
	return 0;
}