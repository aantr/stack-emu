#pragma once

#include <iostream>
#include <iterators.hpp>
#include <defines.hpp>
#include <assert.h>
#include <utility>
#include <cstring>

namespace stack_emu {

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
		vector(std::initializer_list<T>);

		vector(const vector &other); // copy
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
		const_iterator begin() const;
		const_iterator end() const;
		const_iterator cbegin() const;
		const_iterator cend() const;

		reverse_iterator rbegin();
		reverse_iterator rend();
		const_reverse_iterator rbegin() const;
		const_reverse_iterator rend() const;
		const_reverse_iterator crbegin() const;
		const_reverse_iterator crend() const;

		bool operator==(const vector &other) const;
		bool operator!=(const vector &other) const;

	};

}

namespace stack_emu {

	template <class T>
	void vector<T>::reserve_(size_t v) {
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
	void vector<T>::resize_() {
		T* temp = new T[capacity];
		size_t j = std::min(sz, capacity);
		std::copy(data_, data_ + j, temp);
		delete[] data_;
		data_ = temp;
	}

	template <class T>
	vector<T>::vector() {
		sz = 0;
		data_ = new T[0]();
		capacity = 0;
	}

	template <class T>
	vector<T>::vector(unsigned int sz_) {
		sz = 0;
		data_ = new T[0]();
		capacity = 0;
		reserve_(sz_);
	}

	template <class T>
	vector<T>::vector(unsigned int sz_, const T& elem) {
		sz = 0;
		capacity = 0;
		data_ = new T[0]();
		reserve_(sz_);
		std::fill(data_, data_ + sz_, elem);
	}

	template <class T>
	vector<T>::vector(std::initializer_list<T> list) {
		sz = 0;
		capacity = 0;
		data_ = new T[0]();
		reserve_(list.size());
		auto it = list.begin();
		for (size_t i = 0; i < list.size(); i++) {
			data_[i] = *it++;
		}
	}

	template <class T>
	vector<T>::vector(const vector &other) {
		sz = other.sz;
		capacity = other.capacity;
		data_ = new T[capacity]();
		std::copy(other.data_, other.data_ + capacity, data_);
	}

	template <class T>
	vector<T>& vector<T>::operator=(const vector &other) { // copy
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
	vector<T>::vector(vector &&other) noexcept {
		data_ = std::exchange(other.data_, nullptr);
		sz = std::exchange(other.sz, 0);
		capacity = std::exchange(other.capacity, 0);
	}

	template <class T>
	vector<T>& vector<T>::operator=(vector &&other) noexcept {	
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
	vector<T>::~vector() {
		delete[] data_;
	}

	template <class T>
	bool vector<T>::empty() const {
		return sz == 0;
	}

	template <class T>
	size_t vector<T>::size() const {
		return sz;
	}

	template <class T>
	void vector<T>::resize(size_t v) {
		size_t prev_sz = sz;
		reserve_(v);
		if (prev_sz < sz) {
			std::fill(data_ + prev_sz, data_ + sz, T());
		}
	}

	template <class T>
	void vector<T>::resize(size_t v, const T& el) {
		size_t prev_sz = sz;
		reserve_(v);
		if (prev_sz < sz) {
			std::fill(data_ + prev_sz, data_ + sz, el);
		}
	}

	template <class T>
	void vector<T>::pop_back() {
		if (sz == 0) {
			throw std::runtime_error("Pop from empty vector");
		}
		reserve_(sz - 1);
	}

	template <class T>
	void vector<T>::push_back(const T& elem) {
		reserve_(sz + 1);
		data_[sz - 1] = elem;
	}

	template<class T>
	T& vector<T>::front() const {
		if (sz == 0) {
			throw std::runtime_error("Cannot get from empty vector");
		}
		return data_[0];
	}

	template<class T>
	T& vector<T>::back() const {
		if (sz == 0) {
			throw std::runtime_error("Cannot get from empty vector");
		}
		return data_[sz - 1];
	}

	template<class T>
	T& vector<T>::operator[](size_t index) const {
		if (index >= sz) {
			throw std::runtime_error("Index out of range");
		}
		return data_[index];
	}

	template<class T>
	T* vector<T>::data() const {
		return data_;
	}

	template<class T>
	typename vector<T>::iterator vector<T>::begin() {
		return iterator(data_);
	}

	template<class T>
	typename vector<T>::iterator vector<T>::end() {
		return iterator(data_ + sz);
	}

	template<class T>
	typename vector<T>::const_iterator vector<T>::begin() const {
		return const_iterator(data_);
	}

	template<class T>
	typename vector<T>::const_iterator vector<T>::end() const {
		return const_iterator(data_ + sz);
	}

	template<class T>
	typename vector<T>::const_iterator vector<T>::cbegin() const {
		return const_iterator(data_);
	}

	template<class T>
	typename vector<T>::const_iterator vector<T>::cend() const {
		return const_iterator(data_ + sz);
	}

	template<class T>
	typename vector<T>::reverse_iterator vector<T>::rbegin() {
		return reverse_iterator(data_ + sz - 1);
	}

	template<class T>
	typename vector<T>::reverse_iterator vector<T>::rend() {
		return reverse_iterator(data_ - 1);
	}

	template<class T>
	typename vector<T>::const_reverse_iterator vector<T>::rbegin() const {
		return reverse_iterator(data_ + sz - 1);
	}

	template<class T>
	typename vector<T>::const_reverse_iterator vector<T>::rend() const {
		return reverse_iterator(data_ - 1);
	}

	template<class T>
	typename vector<T>::const_reverse_iterator vector<T>::crbegin() const {
		return const_reverse_iterator(data_ + sz - 1);
	}

	template<class T>
	typename vector<T>::const_reverse_iterator vector<T>::crend() const {
		return const_reverse_iterator(data_ - 1);
	}

	template<class T>
	bool vector<T>::operator==(const vector &other) const {		
		if (other.sz != sz) return false;
		for (size_t i = 0; i < sz; i++) {
			if (!(data_[i] == other.data_[i])) {
				return false;
			}
		}
		return true;
	}

	template<class T>
	bool vector<T>::operator!=(const vector &other) const {
		if (other.sz != sz) return true;
		for (size_t i = 0; i < sz; i++) {
			if (data_[i] != other.data_[i]) {
				return true;
			}
		}
		return false;
	}
}