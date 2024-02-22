#pragma once

#include <iostream>
#include <defines.hpp>
#include <stack.hpp>
#include <type_traits>
#include <assert.h>
#include <utility>
#include <cstring>


namespace stack_emu {
	using namespace std;

	template<class T>
	class stack {
	private:
		size_t sz;
		size_t capacity;
		T* data_;

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
		T* data() const;

		bool operator==(const stack &other) const;
		bool operator!=(const stack &other) const;

	};

}

namespace stack_emu {

	template <class T>
	void stack<T>::reserve_(size_t v) {
		bool was = false;
		while (capacity < v) {
			capacity <<= 1;
			if (capacity == 0) {
				capacity++;
			}
			was = true;
		}
		while (capacity > 0 && (capacity >> 2) >= v) {
			capacity >>= 1;
			was = true;
		}
		if (was) {
			resize_();
		}
		sz = v;
	}

	template <class T>
	void stack<T>::resize_() {
		T* temp = new T[capacity];
		size_t j = min(sz, capacity);
        copy(data_, data_ + j, temp);
        delete[] data_;
        data_ = temp;
	}

	template <class T>
	stack<T>::stack() {
		static_assert(is_constructible<T>::value, "is_constructible");
		static_assert(is_copy_constructible<T>::value, "is_copy_constructible");
		sz = 0;
		data_ = new T[0];
		capacity = 0;
	}

	template <class T>
	stack<T>::stack(unsigned int sz_) {
		static_assert(is_constructible<T>::value, "is_constructible");
		static_assert(is_copy_constructible<T>::value, "is_copy_constructible");
		sz = 0;
		data_ = new T[0];
		capacity = 0;
		reserve_(sz_);
	}

	template <class T>
	stack<T>::stack(unsigned int sz_, const T& elem) {
		static_assert(is_constructible<T>::value, "is_constructible");
		static_assert(is_copy_constructible<T>::value, "is_copy_constructible");
		sz = 0;
		capacity = 0;
		data_ = new T[0];
		reserve_(sz_);
		fill(data_, data_ + sz_, elem);	
	}

	template <class T>
	stack<T>::stack(const stack &other) { // copy
		static_assert(is_constructible<T>::value, "is_constructible");
		static_assert(is_copy_constructible<T>::value, "is_copy_constructible");
		sz = other.sz;
		capacity = other.capacity;
		data_ = new T[capacity];
		std::copy(other.data_, other.data_ + capacity, data_);
	}

	template <class T>
	stack<T>& stack<T>::operator=(const stack &other) { // copy
		if (&other == this) {
			return *this;
		}
		delete[] data_;
		sz = other.sz;
		capacity = other.capacity;
		data_ = new T[capacity];
		std::copy(other.data_, other.data_ + capacity, data_);
		return *this;		
	}

	template <class T>
	stack<T>::stack(stack &&other) noexcept {
		static_assert(is_constructible<T>::value, "is_constructible");
		static_assert(is_copy_constructible<T>::value, "is_copy_constructible");
		data_ = exchange(other.data_, nullptr);
		sz = exchange(other.sz, 0);
		capacity = exchange(other.capacity, 0);
	}

	template <class T>
	stack<T>& stack<T>::operator=(stack &&other) noexcept {	
		if (&other == this) {
			return *this;
		}
		delete[] data_;
		data_ = exchange(other.data_, nullptr);
		sz = exchange(other.sz, 0);
		capacity = exchange(other.capacity, 0);
		return *this;
	}

	template <class T>
	stack<T>::~stack() {
		delete[] data_;
	}

	template <class T>
	bool stack<T>::empty() const {
		return sz == 0;
	}

	template <class T>
	size_t stack<T>::size() const {
		return sz;
	}

	template <class T>
	void stack<T>::pop() {
		if (sz == 0) {
			throw runtime_error("Pop from empty stack");
		}
		reserve_(sz - 1);
	}

	template <class T>
	void stack<T>::push(const T& elem) {
		reserve_(sz + 1);
		data_[sz - 1] = elem;
	}

	template<class T>
	T& stack<T>::top() const {
		if (sz == 0) {
			throw runtime_error("Cannot get from empty stack");
		}
		return data_[sz - 1];
	}

	template<class T>
	T* stack<T>::data() const {
		return data_;
	}

	template<class T>
	bool stack<T>::operator==(const stack &other) const {		
		static_assert(is_equality_comparable<T>::value, "is_equality_comparable");
		if (other.sz != sz) return false;
		for (size_t i = 0; i < sz; i++) {
			if (!(data_[i] == other.data_[i])) {
				return false;
			}
		}
 		return true;
	}

	template<class T>
	bool stack<T>::operator!=(const stack &other) const {
		static_assert(is_inequality_comparable<T>::value, "is_inequality_comparable");
		if (other.sz != sz) return true;
		for (size_t i = 0; i < sz; i++) {
			if (data_[i] != other.data_[i]) {
				return true;
			}
		}
 		return false;
	}
}