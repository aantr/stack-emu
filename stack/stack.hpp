#pragma once

#include <iostream>
#include <defines.hpp>
#include <stack.hpp>
#include <type_traits>
#include <assert.h>
#include <utility>
#include <cstring>


namespace stack_emu {

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
		void push(T&&);
		T& top();
		const T& top() const;
		T* data() const;

		bool operator==(const stack &other) const;
		bool operator!=(const stack &other) const;

	};

}

using std::cout;
using std::endl;

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
		T* temp = new T[capacity]();
		size_t j = std::min(sz, capacity);
		#ifdef DEBUG
		cout << "stack resize_ copy:" << endl;
		#endif
		std::copy(data_, data_ + j, temp);
		delete[] data_;
		data_ = temp;
	}

	template <class T>
	stack<T>::stack() {
		sz = 0;
		data_ = new T[0]();
		capacity = 0;
	}

	template <class T>
	stack<T>::stack(unsigned int sz_) {
		sz = 0;
		data_ = new T[0]();
		capacity = 0;
		reserve_(sz_);
	}

	template <class T>
	stack<T>::stack(unsigned int sz_, const T& elem) {
		sz = 0;
		capacity = 0;
		data_ = new T[0]();
		reserve_(sz_);
		#ifdef DEBUG
		cout << "stack fill:" << endl;
		#endif
		fill(data_, data_ + sz_, elem);
	}

	template <class T>
	stack<T>::stack(const stack &other) { // copy
		sz = other.sz;
		capacity = other.capacity;
		data_ = new T[capacity]();
		#ifdef DEBUG
		cout << "stack copy:" << endl;
		#endif
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
		data_ = new T[capacity]();
		std::copy(other.data_, other.data_ + capacity, data_);
		return *this;       
	}

	template <class T>
	stack<T>::stack(stack &&other) noexcept {
		data_ = std::exchange(other.data_, nullptr);
		sz = std::exchange(other.sz, 0);
		capacity = std::exchange(other.capacity, 0);
	}

	template <class T>
	stack<T>& stack<T>::operator=(stack &&other) noexcept { 
		if (&other == this) {
			return *this;
		}
		delete[] data_;
		data_ = std::exchange(other.data_, nullptr);
		sz = std::exchange(other.sz, 0);
		capacity = std::exchange(other.capacity, 0);
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
			throw std::runtime_error("pop from empty stack");
		}
		reserve_(sz - 1);
	}

	template <class T>
	void stack<T>::push(const T& elem) {
		reserve_(sz + 1);
		#ifdef DEBUG
		cout << "stack push lvalue:" << endl;
		#endif
		data_[sz - 1] = elem;
	}

	template <class T>
	void stack<T>::push(T&& elem) {
		reserve_(sz + 1);
		#ifdef DEBUG
		cout << "stack push rvalue:" << endl;
		#endif
		data_[sz - 1] = exchange(elem, T());
	}

	template<class T>
	T& stack<T>::top() {
		if (sz == 0) {
			throw std::runtime_error("cannot get from empty stack");
		}
		return data_[sz - 1];
	}

	template<class T>
	const T& stack<T>::top() const {
		return static_cast<const T&>(*this).top();
	}

	template<class T>
	T* stack<T>::data() const {
		return data_;
	}

	template<class T>
	bool stack<T>::operator==(const stack &other) const {       
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
		if (other.sz != sz) return true;
		for (size_t i = 0; i < sz; i++) {
			if (data_[i] != other.data_[i]) {
				return true;
			}
		}
		return false;
	}
}