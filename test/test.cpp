#include <iostream>
#include <test_system.hpp>
#include <stack.hpp>
#include <vector.hpp>
#include <deque.hpp>
#include <deque>
#include <vector>
#include <arithmetic.hpp>

// using namespace std;
using namespace stack_emu;
using namespace test_system;
using namespace arithmetic;

// test system init && add define
TestSystem ts;
#undef TEST_SYSTEM
#define TEST_SYSTEM ts

void add_tests() {

TEST_ (Init)

	stack_emu::stack<int> s; // empty
	stack_emu::stack<int> s1(5); // 5 zeroes
	// for (int i = 0; i < 5; i++) {
	// 	s1.push(0);
	// }
	stack_emu::stack<int> s2(5, 5); // 5 fives
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

	stack_emu::stack<int> s; // empty
	stack_emu::stack<int> s1(5); // 5 zeroes
	stack_emu::stack<int> s2(5, 5); // 5 fives
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

	stack_emu::stack<int> s; // empty
	stack_emu::stack<int> s1(5); // 5 zeroes
	stack_emu::stack<int> s2(5, 5); // 5 fives
	ASSERT(s != s2);
	for (int i = 0; i < 5; i++) {
		s.push(0);
	}
	for (int i = 0; i < 5; i++) {
		s.pop();
		s1.pop();
		s2.pop();	
	}

	ASSERT(s == s1);



_TEST


TEST_ (FiveRule)

	stack_emu::stack<LongDouble> s; // empty
	stack_emu::stack<LongDouble> s1(5); // 5 zeroes
	stack_emu::stack<LongDouble> s2(5, 5); // 5 fives
	auto s3 = s2; // copy
	ASSERT(s3 == s2);

	stack_emu::stack<LongDouble> s4{std::move(s3)};
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

TEST_ (Vector)

	stack_emu::vector<int> s; // empty
	stack_emu::vector<int> s1(5); // 5 zeroes
	stack_emu::vector<int> s2(5, 5); // 5 fives
	for (int i = 0; i < 5; i++) {
		s2[i]-=i;
	}
	sort(s2.begin(), s2.end());
	auto rs = s2;
	reverse(rs.begin(), rs.end());
	sort(s2.rbegin(), s2.rend());

	ASSERT(s2 == rs);


_TEST


TEST_ (deque)

	stack_emu::deque<int> s; // empty
	stack_emu::deque<int> s1(5); // 5 zeroes
	stack_emu::deque<int> s2(5, 5); // 5 fives
	for (int i = 0; i < 5; i++) {
		s2[i]--;
		ASSERT(s2[i] == 4);
	}
	stack_emu::deque<int> s3 = s2; // copy

	ASSERT(s3 == s2);

	stack_emu::deque<int> s4{std::move(s3)};
	ASSERT(s4 == s2); // move
	ASSERT(s2 != s3); // move effect

	s3 = s2; // copy
	ASSERT(s3 == s2);

	s1.resize(0);
	ASSERT(s1.size() == 0);
	
	for (int i = 0; i < 1000; i++) {
		s1.push_back(1);
		s1.push_front(2);
	}
	for (int i = 0; i < 500; i++) {
		s1.pop_back();
		s1.pop_front();
	}

	s2 = stack_emu::deque<int>(5, 5); // 5 fives

	for (int i = 0; i < 5; i++) {
		s2[i]-=i;
	}
	sort(s2.begin(), s2.end());
	auto rs = s2;
	reverse(rs.begin(), rs.end());
	sort(s2.rbegin(), s2.rend());

	ASSERT(s2 == rs);

_TEST

}

int main() {
	add_tests();
	ts.run_testing(std::cout);
	return 0;
}