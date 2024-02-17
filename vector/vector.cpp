#include <vector.hpp>
#include <defines.hpp>
#include <assert.h>
#include <utility>
#include <cstring>

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
		size_t j = min(sz, capacity);
        for (size_t i = 0; i < j; i++) {
            temp[i] = data_[i];
        }
        for (size_t i = j; i < capacity; i++) {
            temp[i] = T();
        }
        delete[] data_;
        data_ = temp;
	}

	template <class T>
	vector<T>::vector() {
		static_assert(is_constructible<T>::value, "is_constructible");
		static_assert(is_copy_constructible<T>::value, "is_copy_constructible");
		sz = 0;
		data_ = new T[0];
		capacity = 0;
	}

	template <class T>
	vector<T>::vector(unsigned int sz_) {
		static_assert(is_constructible<T>::value, "is_constructible");
		static_assert(is_copy_constructible<T>::value, "is_copy_constructible");
		sz = 0;
		data_ = new T[0];
		capacity = 0;
		reserve_(sz_);
	}

	template <class T>
	vector<T>::vector(unsigned int sz_, const T& elem) {
		static_assert(is_constructible<T>::value, "is_constructible");
		static_assert(is_copy_constructible<T>::value, "is_copy_constructible");
		sz = 0;
		capacity = 0;
		data_ = new T[0];
		reserve_(sz_);
		for (size_t i = 0; i < sz_; i++) {
			data_[i] = elem;
		}
	}

	template <class T>
	vector<T>::vector(const vector &other) { // copy
		static_assert(is_constructible<T>::value, "is_constructible");
		static_assert(is_copy_constructible<T>::value, "is_copy_constructible");
		sz = other.sz;
		capacity = other.capacity;
		data_ = new T[capacity];
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
		data_ = new T[capacity];
		std::copy(other.data_, other.data_ + capacity, data_);
		return *this;		
	}

	template <class T>
	vector<T>::vector(vector &&other) noexcept {
		static_assert(is_constructible<T>::value, "is_constructible");
		static_assert(is_copy_constructible<T>::value, "is_copy_constructible");
		data_ = exchange(other.data_, nullptr);
		sz = exchange(other.sz, 0);
		capacity = exchange(other.capacity, 0);
	}

	template <class T>
	vector<T>& vector<T>::operator=(vector &&other) noexcept {	
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
		auto prev_sz = sz;
		reserve_(v);
		for (auto i = prev_sz; i < sz; i++) {
			data_[i] = T();
		}
	}

	template <class T>
	void vector<T>::resize(size_t v, const T& el) {
		auto prev_sz = sz;
		reserve_(v);
		for (auto i = prev_sz; i < sz; i++) {
			data_[i] = el;
		}
	}

	template <class T>
	void vector<T>::pop_back() {
		if (sz == 0) {
			throw runtime_error("Pop from empty vector");
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
			throw runtime_error("Cannot get from empty vector");
		}
		return data_[0];
	}

	template<class T>
	T& vector<T>::back() const {
		if (sz == 0) {
			throw runtime_error("Cannot get from empty vector");
		}
		return data_[sz - 1];
	}

	template<class T>
	T& vector<T>::operator[](size_t index) const {
		if (index >= sz) {
			throw runtime_error("Index out of range");
		}
		return data_[index];
	}

	template<class T>
	T* vector<T>::data() const {
		return data_;
	}

	template<class T>
	bool vector<T>::operator==(const vector &other) const {		
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
	bool vector<T>::operator!=(const vector &other) const {
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