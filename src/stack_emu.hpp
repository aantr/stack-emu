#pragma once

// #define DEBUG
// #define USE_INT		
#define PRECISION 16
#define MAX_REGISTERS 8
#define USE_CALL_STACK

namespace stack_emu {

	const std::string DELIMITER = "#";

	class compile_error: public std::exception {
	public:
		std::string msg;
		compile_error(const std::string msg_);
		const char* what() const noexcept override;
	};

	class runtime_error: public std::exception {
	public:
		std::string msg;
		runtime_error(const std::string msg_);
		const char* what() const noexcept override;
	};

	bool compile(const char* filename, const char* dest);
	bool emulate(const char* filename);

}


