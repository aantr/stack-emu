#pragma once 

#include <iterator>

namespace stack_emu {
    // iterator def
    template<class T>
    class ra_iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        pointer p;
        ra_iterator(pointer p);

        ra_iterator();
        ra_iterator(const ra_iterator&);
        ra_iterator& operator=(const ra_iterator&);
        ~ra_iterator();
        reference operator*() const;
        ra_iterator& operator++();
        ra_iterator operator++(int);
     
        pointer operator->() const;
     
        ra_iterator& operator--();
        ra_iterator operator--(int);
     
        reference operator[](ptrdiff_t n) const;
        ra_iterator& operator+=(ptrdiff_t n);
        ra_iterator& operator-=(ptrdiff_t n);
    };

    template<class T>
    class ra_reverse_iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        pointer p;
        ra_reverse_iterator(pointer p);

        ra_reverse_iterator();
        ra_reverse_iterator(const ra_reverse_iterator&);
        ra_reverse_iterator& operator=(const ra_reverse_iterator&);
        ~ra_reverse_iterator();
        reference operator*() const;
        ra_reverse_iterator& operator++();
        ra_reverse_iterator operator++(int);
     
        pointer operator->() const;
     
        ra_reverse_iterator& operator--();
        ra_reverse_iterator operator--(int);
     
        reference operator[](ptrdiff_t n) const;
        ra_reverse_iterator& operator+=(ptrdiff_t n);
        ra_reverse_iterator& operator-=(ptrdiff_t n);
    };

    // end iterator
}

// cpp
namespace stack_emu {

    // iterators
    // straight
    template<class T> ra_iterator<T>::ra_iterator(pointer p_): p(p_) {}
    template<class T> ra_iterator<T>::ra_iterator() = default;
    template<class T> ra_iterator<T>::ra_iterator(const ra_iterator<T>&) = default;
    template<class T> ra_iterator<T>& ra_iterator<T>::operator=(const ra_iterator<T>&) = default;
    template<class T> ra_iterator<T>::~ra_iterator() = default;
    template<class T> typename ra_iterator<T>::reference ra_iterator<T>::operator*() const { return *p; }
    template<class T> ra_iterator<T>& ra_iterator<T>::operator++() { p++; return *this; }
    template<class T> ra_iterator<T> ra_iterator<T>::operator++(int) { auto old = *this; ++(*this); return old; }

    template<class T> typename ra_iterator<T>::pointer ra_iterator<T>::operator->() const { return *p; }

    template<class T> ra_iterator<T>& ra_iterator<T>::operator--() { p--; return *this; }
    template<class T> ra_iterator<T> ra_iterator<T>::operator--(int) { auto old = *this; --(*this); return old; }

    template<class T> typename ra_iterator<T>::reference ra_iterator<T>::operator[](ptrdiff_t n) const { auto tmp = *this; tmp += n; return *tmp; }
    template<class T> ra_iterator<T>& ra_iterator<T>::operator+=(ptrdiff_t n) { p += n; return *this; }
    template<class T> ra_iterator<T>& ra_iterator<T>::operator-=(ptrdiff_t n) { return *this += -n; }

    template<class T> void swap(ra_iterator<T>& a, ra_iterator<T>& b) { std::swap(a.p, b.p); }
    template<class T> bool operator==(const ra_iterator<T>& lhs, const ra_iterator<T>& rhs) { return lhs.p == rhs.p; }
    template<class T> bool operator!=(const ra_iterator<T>& lhs, const ra_iterator<T>& rhs) { return !(lhs == rhs); }

    template<class T> bool operator<(const ra_iterator<T>& lhs, const ra_iterator<T>& rhs) { return rhs.p - lhs.p > 0; }
    template<class T> bool operator>(const ra_iterator<T>& lhs, const ra_iterator<T>& rhs) { return rhs < lhs; }
    template<class T> bool operator<=(const ra_iterator<T>& lhs, const ra_iterator<T>& rhs) { return !(rhs > lhs); }
    template<class T> bool operator>=(const ra_iterator<T>& lhs, const ra_iterator<T>& rhs) { return !(lhs < rhs); }
    template<class T> ra_iterator<T> operator+(ra_iterator<T> it, ptrdiff_t n) { it += n; return it; }
    template<class T> ra_iterator<T> operator+(ptrdiff_t n, ra_iterator<T> it) { return it + n; }
    template<class T> ra_iterator<T> operator-(ra_iterator<T> it, ptrdiff_t n) { it -= n; return it; }
    template<class T> ptrdiff_t operator-(const ra_iterator<T>& lhs, const ra_iterator<T>& rhs) { return lhs.p - rhs.p; }

    // reverse
    template<class T> ra_reverse_iterator<T>::ra_reverse_iterator(pointer p_): p(p_) {}
    template<class T> ra_reverse_iterator<T>::ra_reverse_iterator() = default;
    template<class T> ra_reverse_iterator<T>::ra_reverse_iterator(const ra_reverse_iterator<T>&) = default;
    template<class T> ra_reverse_iterator<T>& ra_reverse_iterator<T>::operator=(const ra_reverse_iterator<T>&) = default;
    template<class T> ra_reverse_iterator<T>::~ra_reverse_iterator() = default;
    template<class T> typename ra_reverse_iterator<T>::reference ra_reverse_iterator<T>::operator*() const { return *p; }
    template<class T> ra_reverse_iterator<T>& ra_reverse_iterator<T>::operator++() { p--; return *this; }
    template<class T> ra_reverse_iterator<T> ra_reverse_iterator<T>::operator++(int) { auto old = *this; ++(*this); return old; }

    template<class T> typename ra_reverse_iterator<T>::pointer ra_reverse_iterator<T>::operator->() const { return *p; }

    template<class T> ra_reverse_iterator<T>& ra_reverse_iterator<T>::operator--() { p++; return *this; }
    template<class T> ra_reverse_iterator<T> ra_reverse_iterator<T>::operator--(int) { auto old = *this; --(*this); return old; }

    template<class T> typename ra_reverse_iterator<T>::reference ra_reverse_iterator<T>::operator[](ptrdiff_t n) const { auto tmp = *this; tmp += n; return *tmp; }
    template<class T> ra_reverse_iterator<T>& ra_reverse_iterator<T>::operator+=(ptrdiff_t n) { p -= n; return *this; }
    template<class T> ra_reverse_iterator<T>& ra_reverse_iterator<T>::operator-=(ptrdiff_t n) { return *this += -n; }

    template<class T> void swap(ra_reverse_iterator<T>& a, ra_reverse_iterator<T>& b) { std::swap(a.p, b.p); }
    template<class T> bool operator==(const ra_reverse_iterator<T>& lhs, const ra_reverse_iterator<T>& rhs) { return lhs.p == rhs.p; }
    template<class T> bool operator!=(const ra_reverse_iterator<T>& lhs, const ra_reverse_iterator<T>& rhs) { return !(lhs == rhs); }

    template<class T> bool operator<(const ra_reverse_iterator<T>& lhs, const ra_reverse_iterator<T>& rhs) { return -(rhs.p - lhs.p) > 0; }
    template<class T> bool operator>(const ra_reverse_iterator<T>& lhs, const ra_reverse_iterator<T>& rhs) { return rhs < lhs; }
    template<class T> bool operator<=(const ra_reverse_iterator<T>& lhs, const ra_reverse_iterator<T>& rhs) { return !(rhs > lhs); }
    template<class T> bool operator>=(const ra_reverse_iterator<T>& lhs, const ra_reverse_iterator<T>& rhs) { return !(lhs < rhs); }
    template<class T> ra_reverse_iterator<T> operator+(ra_reverse_iterator<T> it, ptrdiff_t n) { it += n; return it; }
    template<class T> ra_reverse_iterator<T> operator+(ptrdiff_t n, ra_reverse_iterator<T> it) { return it + n; }
    template<class T> ra_reverse_iterator<T> operator-(ra_reverse_iterator<T> it, ptrdiff_t n) { it -= n; return it; }
    template<class T> ptrdiff_t operator-(const ra_reverse_iterator<T>& lhs, const ra_reverse_iterator<T>& rhs) { return -(lhs.p - rhs.p); }

    // end iterator
}