#pragma once

#include <iostream>
#include <deque.hpp>
#include <defines.hpp>
#include <assert.h>
#include <utility>
#include <cstring>
#include <iterators.hpp>

namespace stack_emu {
	using namespace std;

	template<class T>
	class deque {
	private:
		size_t sz;
		size_t capacity;
		size_t begin_;
		T* data_;

		void reserve_(size_t);
		void resize_(size_t);
		void reserve_push_front_();
		void reserve_pop_front_();

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

		deque();
		deque(unsigned int sz);
		deque(unsigned int sz, const T&);
		deque(initializer_list<T>);

		deque(const deque &other); // copy
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

		bool operator==(const deque &other) const;
		bool operator!=(const deque &other) const;

	};

}

namespace stack_emu {

	template <class T>
	void deque<T>::reserve_push_front_() { // makes accessable begin_ - 1
		if (begin_ == 0) {
			resize_(max((size_t) 1u, sz));
		}
	}

	template <class T>
	void deque<T>::reserve_pop_front_() { // resize_ if needed
		if (sz * 2 + 2 <= (capacity >> 1)) {
			resize_(sz);
		}
	}

	// go to the right
	template <class T>
	void deque<T>::reserve_(size_t v) { // makes accessable up to begin_ + v
		if (begin_ + v >= capacity || v * 2 + 2 <= (capacity >> 1)) { // increase or decrease
			resize_(v);
		}
		sz = v;
	}

	/*
	rebuild
	*/
	template <class T>	
	void deque<T>::resize_(size_t v) {
		capacity = v * 2 + 2;
		size_t b = v / 2 + 1;
		T* temp = new T[capacity]();
		size_t j = min(sz, v);
        copy(data_ + begin_, data_ + begin_ + j, temp + b);
        delete[] data_;
        data_ = temp;
        begin_ = b;
	}

	template <class T>
	deque<T>::deque() {
		static_assert(is_constructible<T>::value, "is_constructible");
		static_assert(is_copy_constructible<T>::value, "is_copy_constructible");
		sz = 0;
		begin_ = 0;
		data_ = new T[0]();
		capacity = 0;
	}

	template <class T>
	deque<T>::deque(unsigned int sz_) {
		static_assert(is_constructible<T>::value, "is_constructible");
		static_assert(is_copy_constructible<T>::value, "is_copy_constructible");
		sz = 0;
		begin_ = 0;
		data_ = new T[0]();
		capacity = 0;
		reserve_(sz_);
	}

	template <class T>
	deque<T>::deque(unsigned int sz_, const T& elem) {
		static_assert(is_constructible<T>::value, "is_constructible");
		static_assert(is_copy_constructible<T>::value, "is_copy_constructible");
		sz = 0;
		begin_ = 0;
		capacity = 0;
		data_ = new T[0]();
		reserve_(sz_);
		fill(data_ + begin_, data_ + begin_ + sz_, elem);
	}

	template <class T>
	deque<T>::deque(initializer_list<T> list) {
		static_assert(is_constructible<T>::value, "is_constructible");
		static_assert(is_copy_constructible<T>::value, "is_copy_constructible");
		sz = 0;
		begin_ = 0;
		capacity = 0;
		data_ = new T[0]();
		reserve_(list.size());
		fill(data_ + begin_, data_ + begin_ + list.size(), list.begin());

	}

	template <class T>
	deque<T>::deque(const deque &other) { // copy
		static_assert(is_constructible<T>::value, "is_constructible");
		static_assert(is_copy_constructible<T>::value, "is_copy_constructible");
		sz = other.sz;
		begin_ = other.begin_;
		capacity = other.capacity;
		data_ = new T[capacity]();
		std::copy(other.data_, other.data_ + capacity, data_);
	}

	template <class T>
	deque<T>& deque<T>::operator=(const deque &other) { // copy
		if (&other == this) {
			return *this;
		}
		delete[] data_;
		sz = other.sz;
		begin_ = other.begin_;
		capacity = other.capacity;
		data_ = new T[capacity]();
		std::copy(other.data_, other.data_ + capacity, data_);
		return *this;		
	}

	template <class T>
	deque<T>::deque(deque &&other) noexcept {
		static_assert(is_constructible<T>::value, "is_constructible");
		static_assert(is_copy_constructible<T>::value, "is_copy_constructible");
		data_ = exchange(other.data_, nullptr);
		sz = exchange(other.sz, 0);
		begin_ = exchange(other.begin_, 0);
		capacity = exchange(other.capacity, 0);
	}

	template <class T>
	deque<T>& deque<T>::operator=(deque &&other) noexcept {	
		if (&other == this) {
			return *this;
		}
		delete[] data_;
		data_ = exchange(other.data_, nullptr);
		sz = exchange(other.sz, 0);
		begin_ = exchange(other.begin_, 0);
		capacity = exchange(other.capacity, 0);
		return *this;
	}

	template <class T>
	deque<T>::~deque() {
		delete[] data_;
	}

	template <class T>
	bool deque<T>::empty() const {
		return sz == 0;
	}

	template <class T>
	size_t deque<T>::size() const {
		return sz;
	}

	template <class T>
	void deque<T>::resize(size_t v) {
		size_t prev_sz = sz;
		reserve_(v);
		if (prev_sz < sz) {
			fill(data_ + begin_ + prev_sz, data_ + begin_ + sz, T());
		}
	}

	template <class T>
	void deque<T>::resize(size_t v, const T& elem) {
		size_t prev_sz = sz;
		reserve_(v);
		if (prev_sz < sz) {
			fill(data_ + begin_ + prev_sz, data_ + begin_ + sz, elem);
		}
	}

	template <class T>
	void deque<T>::pop_front() {
		if (sz == 0) {
			throw runtime_error("Pop from empty deque");
		}
		begin_++;
		sz--;
		reserve_pop_front_();
	}

	template <class T>
	void deque<T>::push_front(const T& elem) {
		reserve_push_front_();
		begin_--;
		sz++;
		data_[begin_] = elem;
	}

	template <class T>
	void deque<T>::pop_back() {
		if (sz == 0) {
			throw runtime_error("Pop from empty deque");
		}
		reserve_(sz - 1);
	}

	template <class T>
	void deque<T>::push_back(const T& elem) {
		reserve_(sz + 1);
		data_[begin_ + sz - 1] = elem;
	}

	template<class T>
	T& deque<T>::front() const {
		if (sz == 0) {
			throw runtime_error("Cannot get from empty deque");
		}
		return data_[begin_];
	}

	template<class T>
	T& deque<T>::back() const {
		if (sz == 0) {
			throw runtime_error("Cannot get from empty deque");
		}
		return data_[begin_ + sz - 1];
	}

	template<class T>
	T& deque<T>::operator[](size_t index) const {
		if (index >= sz) {
			throw runtime_error("Index out of range");
		}
		return data_[index + begin_];
	}

	template<class T>
	T* deque<T>::data() const {
		return data_ + begin_;
	}

	template<class T>
	typename deque<T>::iterator deque<T>::begin() {
		return iterator(data_ + begin_);
	}

	template<class T>
	typename deque<T>::iterator deque<T>::end() {
		return iterator(data_ + begin_ + sz);
	}

	template<class T>
	typename deque<T>::const_iterator deque<T>::begin() const {
		return const_iterator(data_ + begin_);
	}

	template<class T>
	typename deque<T>::const_iterator deque<T>::end() const {
		return const_iterator(data_ + begin_ + sz);
	}

	template<class T>
	typename deque<T>::const_iterator deque<T>::cbegin() const {
		return const_iterator(data_ + begin_);
	}

	template<class T>
	typename deque<T>::const_iterator deque<T>::cend() const {
		return const_iterator(data_ + begin_ + sz);
	}

	template<class T>
	typename deque<T>::reverse_iterator deque<T>::rbegin() {
		return reverse_iterator(data_ + begin_ + sz - 1);
	}

	template<class T>
	typename deque<T>::reverse_iterator deque<T>::rend() {
		return reverse_iterator(data_ + begin_ - 1);
	}

	template<class T>
	typename deque<T>::const_reverse_iterator deque<T>::rbegin() const {
		return const_reverse_iterator(data_ + begin_ + sz - 1);
	}

	template<class T>
	typename deque<T>::const_reverse_iterator deque<T>::rend() const {
		return const_reverse_iterator(data_ + begin_ - 1);
	}

	template<class T>
	typename deque<T>::const_reverse_iterator deque<T>::crbegin() const {
		return const_reverse_iterator(data_ + begin_ + sz - 1);
	}

	template<class T>
	typename deque<T>::const_reverse_iterator deque<T>::crend() const {
		return const_reverse_iterator(data_ + begin_ - 1);
	}

	template<class T>
	bool deque<T>::operator==(const deque &other) const {		
		static_assert(is_equality_comparable<T>::value, "is_equality_comparable");
		if (other.sz != sz) return false;
		for (size_t i = begin_; i < begin_ + sz; i++) {
			if (!(data_[i] == other.data_[i])) {
				return false;
			}
		}
 		return true;
	}

	template<class T>
	bool deque<T>::operator!=(const deque &other) const {
		static_assert(is_inequality_comparable<T>::value, "is_inequality_comparable");
		if (other.sz != sz) return true;
		for (size_t i = begin_; i < begin_ + sz; i++) {
			if (data_[i] != other.data_[i]) {
				return true;
			}
		}
 		return false;
	}
}