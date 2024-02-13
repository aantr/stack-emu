#include <iostream>
#include <test_system.hpp>

using namespace test_system;
TestSystem ts;
#undef TEST_SYSTEM
#define TEST_SYSTEM ts

void add_tests() {

TEST_ (Init)


_TEST


}


int main() {
	add_tests();
	ts.run_testing(std::cout);
	return 0;
}