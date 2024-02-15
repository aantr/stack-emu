#include <stack.hpp>
#include <type_traits>
#include <assert.h>
#include <utility>
#include <cstring>

// https://stackoverflow.com/questions/16399346/c11-static-assert-for-equality-comparable-type
template<typename T, typename = void>
struct is_equality_comparable : std::false_type
{ };

template<typename T>
struct is_equality_comparable<T,
    typename std::enable_if<
        true, 
        decltype(std::declval<T&>() == std::declval<T&>(), (void)0)
        >::type
    > : std::true_type
{
};

template<typename T, typename = void>
struct is_inequality_comparable : std::false_type
{ };

template<typename T>
struct is_inequality_comparable<T,
    typename std::enable_if<
        true, 
        decltype(std::declval<T&>() != std::declval<T&>(), (void)0)
        >::type
    > : std::true_type
{
};

namespace stack_emu {

	template <class T>
	void stack<T>::reserve_(size_t v) {
		sz = v;
		while (capacity < v) {
			capacity <<= 1;
			if (capacity == 0) {
				capacity++;
			}
		}
		while (capacity > 0 && (capacity >> 1) >= v) {
			capacity >>= 1;
		}
		resize_();
	}

	template <class T>
	void stack<T>::resize_() {
		T* temp = new T[capacity];
        for (size_t i = 0; i < sz; i++) {
            temp[i] = data[i]; 
        }
        delete[] data;
        data = temp;
	}

	template <class T>
	stack<T>::stack(): sz(0) {
		data = new T[0];
		capacity = 0;
	}

	template <class T>
	stack<T>::stack(unsigned int sz): sz(sz) {
		data = new T[0];
		capacity = 0;
		reserve_(sz);
	}

	template <class T>
	stack<T>::stack(unsigned int sz, const T& elem): sz(sz) {
		capacity = 0;
		data = new T[0];
		reserve_(sz);
		fill(data, data + sz, elem);
	}

	template <class T>
	stack<T>::stack(const stack &other) { // copy
		sz = other.sz;
		capacity = other.capacity;
		data = new T[capacity];
		for (size_t i = 0; i < capacity; i++) {
			data[i] = other.data[i];
		}
	}

	template <class T>
	stack<T>& stack<T>::operator=(const stack &other) { // copy
		if (&other == this) {
			return *this;
		}
		sz = other.sz;
		capacity = other.capacity;
		data = new T[capacity];
		for (size_t i = 0; i < capacity; i++) {
			data[i] = other.data[i];
		}
		return *this;		
	}

	template <class T>
	stack<T>::stack(stack &&other) noexcept {
		data = exchange(other.data, new T[0]);
		sz = exchange(other.sz, 0);
		capacity = exchange(other.capacity, 0);
	}

	template <class T>
	stack<T>& stack<T>::operator=(stack &&other) noexcept {	
		if (&other == this) {
			return *this;
		}
		swap(data, other.data);
		swap(sz, other.sz);
		swap(capacity, other.capacity);
		return *this;
	}

	template <class T>
	stack<T>::~stack() {
		delete[] data;
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
		data[sz - 1] = elem;
	}

	template<class T>
	T& stack<T>::top() const {
		if (sz == 0) {
			throw runtime_error("Cannot get from empty stack");
		}
		return data[sz - 1];
	}

	template<class T>
	bool stack<T>::operator==(const stack &other) const {		
		static_assert(is_equality_comparable<T>::value, "is_equality_comparable");
		if (other.sz != sz) return false;
		for (size_t i = 0; i < sz; i++) {
			if (!(data[i] == other.data[i])) {
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
			if (data[i] != other.data[i]) {
				return true;
			}
		}
 		return false;
	}
}