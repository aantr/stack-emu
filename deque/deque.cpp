#include <deque.hpp>
#include <defines.hpp>
#include <assert.h>
#include <utility>
#include <cstring>

namespace stack_emu {

	template <class T>
	void deque<T>::reserve_push_front_() { // makes accessable begin - 1
		if (begin == 0) {
			resize_(sz);
		}
	}

	template <class T>
	void deque<T>::reserve_pop_front_() { // resize_ if needed
		if (sz * 3 <= (capacity >> 1)) {
			resize_(sz);
		}
	}

	// go to the right
	template <class T>
	void deque<T>::reserve_(size_t v) { // makes accessable up to begin + v
		if (begin + v >= capacity || v * 3 <= (capacity >> 1)) { // increase or decrease
			resize_(v);
		}
		sz = v;
	}

	/*
	rebuild
	*/
	template <class T>	
	void deque<T>::resize_(size_t v) {
		capacity = v * 3;
		T* temp = new T[capacity];
		size_t j = min(sz, v);
        for (size_t i = v; i < v + j; i++) {
            temp[i] = data[i - v + begin];
        }
        for (size_t i = 0; i < v; i++) {
            temp[i] = T();
        }
        for (size_t i = v + j; i < capacity; i++) {
            temp[i] = T();
        }
        delete[] data;
        data = temp;
        begin = v;
	}

	template <class T>
	deque<T>::deque() {
		static_assert(is_constructible<T>::value, "is_constructible");
		static_assert(is_copy_constructible<T>::value, "is_copy_constructible");
		sz = 0;
		begin = 0;
		data = new T[0];
		capacity = 0;
	}

	template <class T>
	deque<T>::deque(unsigned int sz_) {
		static_assert(is_constructible<T>::value, "is_constructible");
		static_assert(is_copy_constructible<T>::value, "is_copy_constructible");
		sz = 0;
		begin = 0;
		data = new T[0];
		capacity = 0;
		reserve_(sz_);
	}

	template <class T>
	deque<T>::deque(unsigned int sz_, const T& elem) {
		static_assert(is_constructible<T>::value, "is_constructible");
		static_assert(is_copy_constructible<T>::value, "is_copy_constructible");
		sz = 0;
		begin = 0;
		capacity = 0;
		data = new T[0];
		reserve_(sz_);
		for (size_t i = 0; i < sz_; i++) {
			data[begin + i] = elem;
		}
	}

	template <class T>
	deque<T>::deque(const deque &other) { // copy
		static_assert(is_constructible<T>::value, "is_constructible");
		static_assert(is_copy_constructible<T>::value, "is_copy_constructible");
		sz = other.sz;
		begin = other.begin;
		capacity = other.capacity;
		data = new T[capacity];
		std::copy(other.data, other.data + capacity, data);
	}

	template <class T>
	deque<T>& deque<T>::operator=(const deque &other) { // copy
		if (&other == this) {
			return *this;
		}
		delete[] data;
		sz = other.sz;
		begin = other.begin;
		capacity = other.capacity;
		data = new T[capacity];
		std::copy(other.data, other.data + capacity, data);
		return *this;		
	}

	template <class T>
	deque<T>::deque(deque &&other) noexcept {
		static_assert(is_constructible<T>::value, "is_constructible");
		static_assert(is_copy_constructible<T>::value, "is_copy_constructible");
		data = exchange(other.data, nullptr);
		sz = exchange(other.sz, 0);
		begin = exchange(other.begin, 0);
		capacity = exchange(other.capacity, 0);
	}

	template <class T>
	deque<T>& deque<T>::operator=(deque &&other) noexcept {	
		if (&other == this) {
			return *this;
		}
		delete[] data;
		data = exchange(other.data, nullptr);
		sz = exchange(other.sz, 0);
		begin = exchange(other.begin, 0);
		capacity = exchange(other.capacity, 0);
		return *this;
	}

	template <class T>
	deque<T>::~deque() {
		delete[] data;
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
		for (size_t i = prev_sz; i < sz; i++) {
			data[i + begin] = T();
		}
	}

	template <class T>
	void deque<T>::resize(size_t v, const T& elem) {
		size_t prev_sz = sz;
		reserve_(v);
		for (size_t i = prev_sz; i < sz; i++) {
			data[i + begin] = elem;
		}
	}

	template <class T>
	void deque<T>::pop_front() {
		if (sz == 0) {
			throw runtime_error("Pop from empty deque");
		}
		begin++;
		sz--;
		reserve_pop_front_();
	}

	template <class T>
	void deque<T>::push_front(const T& elem) {
		reserve_push_front_();
		begin--;
		sz++;
		data[begin] = elem;
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
		data[begin + sz - 1] = elem;
	}

	template<class T>
	T& deque<T>::front() const {
		if (sz == 0) {
			throw runtime_error("Cannot get from empty deque");
		}
		return data[begin];
	}

	template<class T>
	T& deque<T>::back() const {
		if (sz == 0) {
			throw runtime_error("Cannot get from empty deque");
		}
		return data[begin + sz - 1];
	}

	template<class T>
	T& deque<T>::operator[](size_t index) const {
		if (index >= sz) {
			throw runtime_error("Index out of range");
		}
		return data[index + begin];
	}


	template<class T>
	bool deque<T>::operator==(const deque &other) const {		
		static_assert(is_equality_comparable<T>::value, "is_equality_comparable");
		if (other.sz != sz) return false;
		for (size_t i = begin; i < begin + sz; i++) {
			if (!(data[i] == other.data[i])) {
				return false;
			}
		}
 		return true;
	}

	template<class T>
	bool deque<T>::operator!=(const deque &other) const {
		static_assert(is_inequality_comparable<T>::value, "is_inequality_comparable");
		if (other.sz != sz) return true;
		for (size_t i = begin; i < begin + sz; i++) {
			if (data[i] != other.data[i]) {
				return true;
			}
		}
 		return false;
	}
}