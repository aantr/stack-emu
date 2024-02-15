#include <test_system.hpp>

namespace test_system {

	Test::Test(string name, function<bool()> test): name(name), test(test) {

	}

	void TestSystem::add_test(Test test) {
		tests.push_back(test);
	}

	void TestSystem::run_testing(ostream& os) {
		int failed = 0;
		for (Test test: tests) {
			try {
				os << YELLOW + "Test" + RESET + " \"" << test.name << "\" ... ";
				os.flush();
				test.test();
				os << GREEN + "passed" + RESET << std::endl;
			} catch (const char *str) {
				os << RED + "failed" + RESET + ", expression: (" << str << ")\n";
				os.flush();
				failed++;
			} catch (const exception &exc) {
				os << RED + "failed" + RESET + ", error: (" << exc.what() << ")\n";
				os.flush();
				failed++;
			}
		}
		if (failed) {
			if (failed == 1) {
				os << failed << " test " + BRED + "failed\n" + RESET;
				os.flush();
			} else {
				os << failed << " tests " + BRED + "failed\n" + RESET;
				os.flush();
			}
		} else {
			os << BGREEN + "All tests passed\n" + RESET;
			os.flush();
		}
	}

}
