#pragma once

#include <iostream>
#include <iterators.cpp>

namespace stack_emu {
	using namespace std;

	template<class T>
	class vector {
	private:
		size_t sz;
		size_t capacity;
		T* data_;

		void reserve_(size_t);
		void resize_();

	public:
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef T& reference;
		typedef const T& const_reference;
		typedef int difference_type;
		typedef size_t size_type;
		typedef T value_type;
		typedef ra_iterator<T> iterator;
    	typedef ra_iterator<const T> const_iterator;
    	typedef ra_reverse_iterator<T> reverse_iterator;
    	typedef ra_reverse_iterator<const T> const_reverse_iterator;

		vector();
		vector(unsigned int sz);
		vector(unsigned int sz, const T&);

		vector(const vector &ohter); // copy
		vector& operator=(const vector &other); // copy
		vector(vector &&other) noexcept; // move
		vector& operator=(vector &&other) noexcept; // move

		~vector();

		size_t size() const;
		void resize(size_t);
		void resize(size_t, const T&);
		bool empty() const;
		void pop_back();
		void push_back(const T&);
		T& front() const;
		T& back() const;
		T& operator[](size_t) const;
		T* data() const;

		iterator begin();
		iterator end();
		const_iterator cbegin() const;
		const_iterator cend() const;

		reverse_iterator rbegin();
		reverse_iterator rend();
		const_reverse_iterator crbegin() const;
		const_reverse_iterator crend() const;

		bool operator==(const vector &other) const;
		bool operator!=(const vector &other) const;

	};

};