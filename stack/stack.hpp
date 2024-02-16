#pragma once

#include <iostream>

namespace stack_emu {
	using namespace std;

	template<class T>
	class stack {
	private:
		size_t sz;
		size_t capacity;
		T* data;

		void reserve_(size_t);
		void resize_();

	public:
		stack();
		stack(unsigned int sz);
		stack(unsigned int sz, const T&);

		stack(const stack &ohter); // copy
		stack& operator=(const stack &other); // copy
		stack(stack &&other) noexcept; // move
		stack& operator=(stack &&other) noexcept; // move

		~stack();

		size_t size() const;
		bool empty() const;
		void pop();
		void push(const T&);
		T& top() const;

		bool operator==(const stack &other) const;
		bool operator!=(const stack &other) const;

	};

};