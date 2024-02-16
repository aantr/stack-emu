#pragma once

#include <iostream>

namespace stack_emu {
	using namespace std;

	template<class T>
	class vector {
	public:
		size_t sz;
		size_t capacity;
		T* data;

		void reserve_(size_t);
		void resize_(size_t);

	public:
		vector();
		vector(unsigned int sz);
		vector(unsigned int sz, const T&);

		vector(const vector &ohter); // copy
		vector& operator=(const vector &other); // copy
		vector(vector &&other) noexcept; // move
		vector& operator=(vector &&other) noexcept; // move

		~vector();

		size_t size() const;
		bool empty() const;
		void pop_back();
		void push_back(const T&);
		T& front() const;
		T& back() const;
		T& operator[](size_t) const;

		bool operator==(const vector &other) const;
		bool operator!=(const vector &other) const;

	};

};