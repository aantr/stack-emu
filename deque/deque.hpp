#pragma once

#include <iostream>

namespace stack_emu {
	using namespace std;

	template<class T>
	class deque {
	private:
		size_t sz;
		size_t capacity;
		size_t begin;
		T* data;

		void reserve_(size_t);
		void resize_(size_t);
		void reserve_push_front_();
		void reserve_pop_front_();

	public:
		deque();
		deque(unsigned int sz);
		deque(unsigned int sz, const T&);

		deque(const deque &ohter); // copy
		deque& operator=(const deque &other); // copy
		deque(deque &&other) noexcept; // move
		deque& operator=(deque &&other) noexcept; // move

		~deque();

		size_t size() const;
		void resize(size_t);
		void resize(size_t, const T&);
		bool empty() const;
		void pop_back();
		void push_back(const T&);
		void pop_front();
		void push_front(const T&);
		T& front() const;
		T& back() const;
		T& operator[](size_t) const;

		bool operator==(const deque &other) const;
		bool operator!=(const deque &other) const;

	};

};