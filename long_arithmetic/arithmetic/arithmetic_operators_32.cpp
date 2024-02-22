#include <defines.hpp>

#include <iostream>
#include <cstring>
#include <string>
#include <assert.h>
#include <iomanip>
#include <arithmetic_32.hpp>
#include <fft.hpp>
#include <climits>

#define HALF_FFT
// #define QUARTER_FFT

namespace arithmetic_32 {

    using namespace std;

    fft::fft fft;

    ostream& operator<<(ostream& os, const LongDouble& value) {
        if (value.sign == -1)
            os << '-';
        // os << "0x";
        if (value.digits_size == 0) {
            os << '0';
            return os;
        }

        int shift = (value.exponent % 4 + 4) % 4;
        auto get_digit = [&](int index) -> digit {
            if (index < 0 || index >= value.digits_size) return 0;
            return value.digits[index];
        };
        auto get_correct_hex = [&](int index) -> digit {
            // [index * 4 + 3 - shift ... index * 4, (index - 1) * 4 + 3 ... (index - 1) * 4 + 4 - shift]
            int digit_index = (index * 4 + 3 - shift) >> 5;
            int digit_index2 = ((index - 1) * 4 + 3) >> 5;
            return ((get_digit(digit_index) >> ((index * 4) & 31) & LongDouble::ones[4 - shift]) << shift) | 
                        (get_digit(digit_index2) >> (((index - 1) * 4 + 4 - shift) & 31) & LongDouble::ones[shift]);
        };
        auto get_char = [&](int value_) -> char {
            if (value_ < 10) return (char) ('0' + value_);
            return (char) ('a' + value_ - 10);
        };

        int digits_size_hex = (value.digits_size << 3) + 1;
        while (digits_size_hex > 0 && get_correct_hex(digits_size_hex - 1) == 0) {
            digits_size_hex--;
        }
        int left = max(0, digits_size_hex - (int) os.precision());
        while (LongDouble::output_insignificant_zeroes == false && left < digits_size_hex && get_correct_hex(left) == 0) {
            left++;
        }
        int exponent = (value.exponent - shift) / 4;
        for (int i = -exponent; i > digits_size_hex - 1; i--) {
            os << '0';
            if (i == -exponent) os << '.';
        }
        for (int i = digits_size_hex - 1; i >= left; i--) {
            os << get_char(get_correct_hex(i));
            if (i > left && i == -exponent) os << '.';
        }
        for (int i = 0; i < exponent + left; i++) {
            os << '0';
        }
        return os;
    }

    LongDouble& LongDouble::operator=(const LongDouble& other) { // C5267
        if (this == &other) {
            return *this;
        }
        sign = other.sign;
        digits_size = other.digits_size;
        free(digits);
        digits = (digit*) malloc(other.digits_size * sizeof(digit));
        if (!digits) memory_error();
        memcpy(digits, other.digits, other.digits_size * sizeof(digit));
        assert(digits != other.digits);
        precision = other.precision;
        exponent = other.exponent;
        return *this;
    }

    LongDouble& LongDouble::operator=(LongDouble &&other) noexcept {
        if (this == &other) {
            return *this;
        }
        free(digits);
        sign = exchange(other.sign, 1);
        digits = exchange(other.digits, nullptr);
        digits_size = exchange(other.digits_size, 0);
        precision = exchange(other.precision, default_precision);
        exponent = exchange(other.exponent, 0);
        return *this;
    }

    LongDouble operator""_ld (long double x) {
        LongDouble res = x;
        return res;
    }

    LongDouble operator""_ld (unsigned long long x) {
        LongDouble res = (uint64_t) x;
        return res;
    }

    LongDouble LongDouble::operator<<(unsigned int val) const { // todo: int >> exceptions
        LongDouble res = *this;
        res <<= val;
        return res;
    }

    void LongDouble::operator<<=(unsigned int val) {
        if (!isInt()) {
            int_error();
        }
        if (!isZero()) exponent += val;
    }

    LongDouble LongDouble::operator>>(unsigned int val) const {
        LongDouble res = *this;
        res >>= val;
        return res;
    }

    void LongDouble::operator>>=(unsigned int val) {
        if (!isInt()) {
            int_error();
        }
        int e = min((unsigned int) exponent, val);
        exponent -= e;
        val -= e;
        unsigned int d = val >> 5;
        if (d >= (unsigned int) digits_size) {
            *this = LongDouble(0, precision);
            return;
        }
        removeFirst(d);
        exponent -= val;
        d = val - (d << 5);
        assert(d < 32);
        if (digits_size != 0) {
            digits[0] &= (ones[32] - ones[d]);
            removeRightZeroes(); 
        }

    }

    LongDouble LongDouble::operator*(const LongDouble& x) const {
        LongDouble res = *this;
        res *= x;
        return res;
    }

    void LongDouble::operator*=(const LongDouble& other) {

        LongDouble x = other;

        sign = sign * x.sign;
        exponent = exponent + x.exponent;

        fft::digit* res;
        int res_size = 0;

        // cut on a half blocks
        #if defined(HALF_FFT) || defined(QUARTER_FFT)
        auto cut = [](digit* arr, int size) -> pair<fft::digit*, int> {
            if (size == 0) {
                fft::digit* res_ = (fft::digit*) malloc(size * sizeof(fft::digit));
                memcpy(res_, arr, size * sizeof(fft::digit));
                return {res_, size};
            }
            #ifdef HALF_FFT
            int newsize = size * 2;
            fft::digit* res_ = (fft::digit*) malloc(newsize * sizeof(fft::digit));
            for (int i = 0; i < size; i++) {
                res_[i * 2] = arr[i] & ones[16];
                res_[i * 2 + 1] = arr[i] >> 16;
            }
            return {res_, newsize};
            #endif

            #ifdef QUARTER_FFT
            int newsize = size * 4;
            fft::digit* res_ = (fft::digit*) malloc(newsize * sizeof(fft::digit));
            for (int i = 0; i < size; i++) {
                res_[i * 4] = arr[i] & ones[8];
                res_[i * 4 + 1] = (arr[i] & ones[16]) >> 8;
                res_[i * 4 + 2] = (arr[i] & ones[24]) >> 16;
                res_[i * 4 + 3] = (arr[i] & ones[32]) >> 24;
            }
            return {res_, newsize};
            #endif
        };
        auto merge = [](fft::digit* arr, int size) -> pair<digit*, int> {
            if (size == 0) {
                digit* res_ = (digit*) malloc(size * sizeof(digit));
                for (int i = 0; i < size; i++) {
                    res_[i] = arr[i];
                }
                return {res_, size};
            }
            #ifdef HALF_FFT
            int newsize = (size - 1) / 2 + 1;
            digit* res_ = (digit*) malloc(newsize * sizeof(digit));
            for (int i = 0; i < size; i++) {
                if (i % 2 == 0) res_[i / 2] = arr[i];
                else res_[i / 2] |= (digit) arr[i] << 16;
            }
            return {res_, newsize};
            #endif
            #ifdef QUARTER_FFT
            int newsize = (size - 1) / 4 + 1;
            digit* res_ = (digit*) malloc(newsize * sizeof(digit));
            for (int i = 0; i < size; i++) {
                if (i % 4 == 0) res_[i / 4] = arr[i];
                else if (i % 4 == 1) res_[i / 4] |= arr[i] << 8;
                else if (i % 4 == 2) res_[i / 4] |= arr[i] << 16;
                else if (i % 4 == 3) res_[i / 4] |= arr[i] << 24;
            }
            return {res_, newsize};
            #endif
        };
        auto [first, first_size] = cut(digits, digits_size);
        free(digits);

        auto [second, second_size] = cut(x.digits, x.digits_size);
        #ifdef HALF_FFT
        fft.multiply(first, first_size, second, second_size, res, res_size, 1 << 16);
        #elif defined(QUARTER_FFT)
        fft.multiply(first, first_size, second, second_size, res, res_size, 1 << 8);
        #endif
        free(first);
        free(second);
        auto [newres, newressize] = merge(res, res_size);
        free(res);

        digits = newres;
        digits_size = newressize;

        #else
        
        // dont cut
        fft.multiply(digits, digits_size, x.digits, x.digits_size, res, res_size, LongDouble::base);
        free(digits);
        digits = res;
        digits_size = res_size;

        #endif

        removeZeroes();
        if (digits_size > precision) {
            removeFirst(digits_size - precision);
        }
        removeZeroes();

    }

    LongDouble LongDouble::operator-() const {
        LongDouble res(*this, precision);
        res.sign = -res.sign;
        return res;
    }

    bool LongDouble::operator<(const LongDouble& x) const {
        if (isZero()) return x.sign == 1 && !x.isZero();
        if (x.isZero()) return sign == -1 && !isZero();
        if (sign != x.sign) {
            return sign < x.sign && (!isZero() || !x.isZero());
        }

        assert(digits[digits_size - 1] != 0 && x.digits[x.digits_size - 1] != 0);

        int digits_size_2 = digits_size << 5;
        int left = 0;
        while (left < 32 && (digits[digits_size - 1] >> (31 - left) & 1) == 0) {
            digits_size_2--;
            left++;
        }

        int x_digits_size_2 = x.digits_size << 5;
        left = 0;
        while (left < 32 && (x.digits[x.digits_size - 1] >> (31 - left) & 1) == 0) {
            x_digits_size_2--;
            left++;
        }   
        if (digits_size_2 + exponent != x_digits_size_2 + x.exponent) {
            return (digits_size_2 + exponent < x_digits_size_2 + x.exponent) ^ (sign == -1);
        }

        int exp_mod = (digits_size << 5) - digits_size_2 - ((x.digits_size << 5) - x_digits_size_2);
        if (exp_mod >= 0) {
            for (int i = 0; i < min(digits_size, x.digits_size); i++) {
                digit next_d = (digits_size - 1 - i - 1 >= 0 && exp_mod ? digits[digits_size - 1 - i - 1] >> (32 - (exp_mod)) : 0);
                if ((((digits[digits_size - 1 - i] & ones[32 - exp_mod]) << exp_mod) | next_d) > x.digits[x.digits_size - 1 - i]) return false ^ (sign == -1);                
                if ((((digits[digits_size - 1 - i] & ones[32 - exp_mod]) << exp_mod) | next_d) < x.digits[x.digits_size - 1 - i]) return true ^ (sign == -1);                
            }
        } else {
            for (int i = 0; i < min(digits_size, x.digits_size); i++) {
                digit next_d = (x.digits_size - 1 - i - 1 >= 0 && exp_mod ? x.digits[x.digits_size - 1 - i - 1] >> (32 - (-exp_mod)) : 0);
                if (digits[digits_size - 1 - i] > (((x.digits[x.digits_size - 1 - i] & ones[32 + exp_mod]) << (-exp_mod)) | next_d)) return false ^ (sign == -1);
                if (digits[digits_size - 1 - i] < (((x.digits[x.digits_size - 1 - i] & ones[32 + exp_mod]) << (-exp_mod)) | next_d)) return true ^ (sign == -1);
            }   
        }
        for (int i = min(digits_size, x.digits_size); i < digits_size; i++) {
            if (i == min(digits_size, x.digits_size) && exp_mod > 0) {
                if ((digits[digits_size - 1 - i] & ones[32 - exp_mod]) != 0) {
                    return false ^ (sign == -1);
                }
            } else {
                if (digits[digits_size - 1 - i] != 0) {
                    return false ^ (sign == -1);
                }
            }
        }
        for (int i = min(digits_size, x.digits_size); i < x.digits_size; i++) {
            if (i == min(digits_size, x.digits_size) && -(exp_mod) > 0) {
                if ((x.digits[x.digits_size - 1 - i] & ones[32 + exp_mod]) != 0) {
                    return true ^ (sign == -1);
                }
            } else {
                if (x.digits[x.digits_size - 1 - i] != 0) {
                    return true ^ (sign == -1);
                }
            }
        }
        return false;
    }

    bool LongDouble::operator==(const LongDouble& x) const {
        if (sign != x.sign) {
            return (isZero() && x.isZero()); 
        }
        if (isZero() || x.isZero()) {
            return isZero() && x.isZero();
        }

        assert(digits[digits_size - 1] != 0 && x.digits[x.digits_size - 1] != 0);

        int digits_size_2 = digits_size << 5;
        int left = 0;
        while (left < 32 && (digits[digits_size - 1] >> (31 - left) & 1) == 0) {
            digits_size_2--;
            left++;
        }

        int x_digits_size_2 = x.digits_size << 5;
        left = 0;
        while (left < 32 && (x.digits[x.digits_size - 1] >> (31 - left) & 1) == 0) {
            x_digits_size_2--;
            left++;
        }

        if (digits_size_2 + exponent != x_digits_size_2 + x.exponent) {
            return false;
        }

        int exp_mod = (digits_size << 5) - digits_size_2 - ((x.digits_size << 5) - x_digits_size_2);

        if (exp_mod >= 0) {
            for (int i = 0; i < min(digits_size, x.digits_size); i++) {
                digit next_d = (digits_size - 1 - i - 1 >= 0 && exp_mod ? digits[digits_size - 1 - i - 1] >> (32 - (exp_mod)) : 0);
                if ((((digits[digits_size - 1 - i] & ones[32 - exp_mod]) << exp_mod) | next_d) != x.digits[x.digits_size - 1 - i]) {
                    return false;
                }
            }
        } else {
            for (int i = 0; i < min(digits_size, x.digits_size); i++) {
                digit next_d = (x.digits_size - 1 - i - 1 >= 0 && exp_mod ? x.digits[x.digits_size - 1 - i - 1] >> (32 - (-exp_mod)) : 0);
                if (digits[digits_size - 1 - i] != (((x.digits[x.digits_size - 1 - i] & ones[32 + exp_mod]) << (-exp_mod)) | next_d)) return false;
            }   
        }
        for (int i = min(digits_size, x.digits_size); i < digits_size; i++) {
            if (i == min(digits_size, x.digits_size) && exp_mod > 0) {
                if ((digits[digits_size - 1 - i] & ones[32 - exp_mod]) != 0) {
                    return false;
                }
            } else {
                if (digits[digits_size - 1 - i] != 0) {
                    return false;
                }
            }
        }
        for (int i = min(digits_size, x.digits_size); i < x.digits_size; i++) {
            if (i == min(digits_size, x.digits_size) && -(exp_mod) > 0) {
                if ((x.digits[x.digits_size - 1 - i] & ones[32 + exp_mod]) != 0) {
                    return false;
                }
            } else {
                if (x.digits[x.digits_size - 1 - i] != 0) {
                    return false;
                }
            }
        }
        return true;
    }

    bool LongDouble::operator!=(const LongDouble& x) const {
        return !(*this == x);
    }

    bool LongDouble::operator>(const LongDouble& x) const {
        return x < *this;
    }

    bool LongDouble::operator<=(const LongDouble& x) const {
        return *this < x || *this == x;
    }

    bool LongDouble::operator>=(const LongDouble& x) const {
        return *this > x || *this == x;
    }

    #define SUM(a, b) (a += b, a - b <= a)
    #define SUB(a, b) (a -= b, a + b >= a)

    LongDouble LongDouble::operator+(const LongDouble& x) const {
        if (sign == x.sign) {
            LongDouble res;
            res.precision = precision;
            res.sign = sign;

            int exp_div = exponent / 32, exp_mod = exponent % 32;
            if (exp_mod < 0) exp_mod += 32, exp_div--;
            int x_exp_div = x.exponent / 32, x_exp_mod = x.exponent % 32;
            if (x_exp_mod < 0) x_exp_mod += 32, x_exp_div--;

            int res_size = max(digits_size + exp_div, x.digits_size + x_exp_div) - min(exp_div, x_exp_div) + 1;
            digit* resd = (digit*) malloc(res_size * sizeof(digit));
            if (!resd) memory_error();
            memset(resd, 0, res_size * sizeof(digit));
            int carry = 0;
            if (exp_div <= x_exp_div) {
                res.exponent = exp_div * 32;
                digit next_d;
                for (int i = 0; i < digits_size; i++) {
                    next_d = (i - 1 >= 0 && exp_mod ? digits[i - 1] >> (32 - (exp_mod)) : 0);
                    resd[i] += (((digits[i] & ones[32 - exp_mod]) << exp_mod) | next_d); // + exp_mod
                }
                next_d = (digits_size - 1 >= 0 && exp_mod ? digits[digits_size - 1] >> (32 - (exp_mod)) : 0);
                resd[digits_size] += next_d;

                for (int i = 0; i < x.digits_size; i++) {
                    next_d = (i - 1 >= 0 && x_exp_mod ? x.digits[i - 1] >> (32 - (x_exp_mod)) : 0);
                    digit y = ((x.digits[i] & ones[32 - x_exp_mod]) << x_exp_mod) | next_d;
                    bool c = SUM(y, carry);
                    if (!c || !SUM(resd[i + x_exp_div - exp_div], y)) {
                        carry = 1;
                    } else {
                        carry = 0;
                    }
                }

                next_d = (x.digits_size - 1 >= 0 && x_exp_mod ? x.digits[x.digits_size - 1] >> (32 - (x_exp_mod)) : 0);
                bool c = SUM(next_d, carry);
                if (!c || !SUM(resd[x.digits_size + x_exp_div - exp_div], next_d)) {
                    carry = 1;
                } else {
                    carry = 0;
                }
                for (int i = x.digits_size + x_exp_div - exp_div + 1; i < res_size; i++) {
                    if (!carry) {
                        break;
                    }
                    if (resd[i] == ones[32]) {
                        carry = 1;
                    } else {
                        carry = 0;
                    }
                    resd[i]++;
                }

            } else {
                res.exponent = x_exp_div * 32;
                digit next_d;
                for (int i = 0; i < x.digits_size; i++) {
                    next_d = (i - 1 >= 0 && x_exp_mod ? x.digits[i - 1] >> (32 - (x_exp_mod)) : 0);
                    resd[i] += ((x.digits[i] & ones[32 - x_exp_mod]) << x_exp_mod) | next_d;
                }
                next_d = (x.digits_size - 1 >= 0 && x_exp_mod ? x.digits[x.digits_size - 1] >> (32 - (x_exp_mod)) : 0);
                resd[x.digits_size] += next_d;
                for (int i = 0; i < digits_size; i++) {
                    next_d = (i - 1 >= 0 && exp_mod ? digits[i - 1] >> (32 - (exp_mod)) : 0);
                    digit y = (((digits[i] & ones[32 - exp_mod]) << exp_mod) | next_d);
                    bool c = SUM(y, carry);
                    if (!c || !SUM(resd[i + exp_div - x_exp_div], y)) {
                        carry = 1;
                    } else {
                        carry = 0;
                    }
                }
                next_d = (digits_size - 1 >= 0 && exp_mod ? digits[digits_size - 1] >> (32 - (exp_mod)) : 0);
                bool c = SUM(next_d, carry);
                if (!c || !SUM(resd[digits_size + exp_div - x_exp_div], next_d)) {
                    carry = 1;
                } else {
                    carry = 0;
                }
                for (int i = digits_size + exp_div - x_exp_div + 1; i < res_size; i++) {
                    if (!carry) {
                        break;
                    }
                    if (resd[i] == ones[32]) {
                        carry = 1;
                    } else {
                        carry = 0;
                    }
                    resd[i]++;
                }
            }
            assert(carry == 0);
            free(res.digits);
            res.digits = resd;
            res.digits_size = res_size;
            res.removeZeroes();
            #ifdef PRECISION_ADD
            if (res.digits_size > res.precision) {
                res.removeFirst(res.digits_size - res.precision);
            }
            res.removeZeroes();
            #endif

            return res;
        }

        if (sign == -1)
            return x - (-(*this));

        return *this - (-x); 
    }

    void LongDouble::operator+=(const LongDouble& x) {
        *this = *this + x;
    }  

    LongDouble LongDouble::operator-(const LongDouble& x) const {
        if (sign == 1 && x.sign == 1) {
            if (*this < x) {
                LongDouble res = x - *this;
                return -res;
            }

            LongDouble res;
            res.sign = 1;
            res.precision = precision;

            int exp_div = exponent / 32, exp_mod = exponent % 32;
            if (exp_mod < 0) exp_mod += 32, exp_div--;
            int x_exp_div = x.exponent / 32, x_exp_mod = x.exponent % 32;
            if (x_exp_mod < 0) x_exp_mod += 32, x_exp_div--;

            int res_size = max(digits_size + exp_div, x.digits_size + x_exp_div) - min(exp_div, x_exp_div) + 1;
            digit* resd = (digit*) malloc(res_size * sizeof(digit));
            if (!resd) memory_error();
            memset(resd, 0, res_size * sizeof(digit));
            int carry = 0;
            if (exp_div <= x_exp_div) {
                res.exponent = exp_div * 32;
                digit next_d;
                for (int i = 0; i < digits_size; i++) {
                    next_d = (i - 1 >= 0 && exp_mod ? digits[i - 1] >> (32 - (exp_mod)) : 0);
                    resd[i] += (((digits[i] & ones[32 - exp_mod]) << exp_mod) | next_d); // + exp_mod
                }
                next_d = (digits_size - 1 >= 0 && exp_mod ? digits[digits_size - 1] >> (32 - (exp_mod)) : 0);
                resd[digits_size] += next_d;

                for (int i = 0; i < x.digits_size; i++) {
                    next_d = (i - 1 >= 0 && x_exp_mod ? x.digits[i - 1] >> (32 - (x_exp_mod)) : 0);
                    digit y = ((x.digits[i] & ones[32 - x_exp_mod]) << x_exp_mod) | next_d;
                    bool c = SUM(y, carry);
                    if (!c || !SUB(resd[i + x_exp_div - exp_div], y)) {
                        carry = 1;
                    } else {
                        carry = 0;
                    }
                }
                next_d = (x.digits_size - 1 >= 0 && x_exp_mod ? (x.digits[x.digits_size - 1] >> (32 - (x_exp_mod))) : 0);
                bool c = SUM(next_d, carry);
                if (!c || !SUB(resd[x.digits_size + x_exp_div - exp_div], next_d)) {
                    carry = 1;
                } else {
                    carry = 0;
                }
                for (int i = x.digits_size + x_exp_div - exp_div + 1; i < res_size; i++) {
                    if (!carry) {
                        break;
                    }
                    if (resd[i] == 0) {
                        carry = 1;
                    } else {
                        carry = 0;
                    }
                    resd[i]--;
                }

            } else {
                res.exponent = x_exp_div * 32;
                digit next_d;
                for (int i = 0; i < digits_size; i++) {
                    next_d = (i - 1 >= 0 && exp_mod ? digits[i - 1] >> (32 - (exp_mod)) : 0);
                    resd[i + exp_div - x_exp_div] += (((digits[i] & ones[32 - exp_mod]) << exp_mod) | next_d);
                }
                next_d = (digits_size - 1 >= 0 && exp_mod ? digits[digits_size - 1] >> (32 - (exp_mod)) : 0);
                resd[digits_size + exp_div - x_exp_div] += next_d;

                for (int i = 0; i < x.digits_size; i++) {
                    next_d = (i - 1 >= 0 && x_exp_mod ? x.digits[i - 1] >> (32 - (x_exp_mod)) : 0);
                    digit y = ((x.digits[i] & ones[32 - x_exp_mod]) << x_exp_mod) | next_d;
                    bool c = SUM(y, carry);
                    if (!c || !SUB(resd[i], y)) {
                        carry = 1;
                    } else {
                        carry = 0;
                    }
                }
                
                next_d = (x.digits_size - 1 >= 0 && x_exp_mod ? x.digits[x.digits_size - 1] >> (32 - (x_exp_mod)) : 0);
                bool c = SUM(next_d, carry);
                if (!c || !SUB(resd[x.digits_size], next_d)) {
                    carry = 1;
                } else {
                    carry = 0;
                }
                for (int i = x.digits_size + 1; i < res_size; i++) {
                    if (!carry) {
                        break;
                    }
                    if (resd[i] == 0) {
                        carry = 1;
                    } else {
                        carry = 0;
                    }
                    resd[i]--;
                }
            }

            assert(carry == 0);

            free(res.digits);

            res.digits = resd;
            res.digits_size = res_size;

            res.removeZeroes();
            #ifdef PRECISION_SUB
            if (res.digits_size > res.precision) {
                res.removeFirst(res.digits_size - res.precision);
            }
            res.removeZeroes();
            #endif
            return res;
        }

        if (sign == -1 && x.sign == -1)
            return (-x) - (-(*this));

        return *this + (-x);
    }

    void LongDouble::operator-=(const LongDouble& x) {
        *this = *this - x;
    }

    LongDouble LongDouble::operator/(const LongDouble& other) const {

        LongDouble x (*this);
        assert(x.isZero() || x.digits[x.digits_size - 1] != 0);
        assert(other.isZero() || other.digits[other.digits_size - 1] != 0);

        x.sign = 1;
        LongDouble y (other);
        y.sign = 1;
        int res_exponent = x.exponent - y.exponent;
        x.exponent = 0;
        y.exponent = 0;
        int plus = max(0, precision - x.digits_size + y.digits_size + 2);

        if (!x.isZero()) {
            int temp_size = plus + x.digits_size;
            digit* temp = (digit*) malloc(temp_size * sizeof(digit));
            if (!temp) memory_error();
            memset(temp, 0, plus * sizeof(digit));
            memcpy(temp + plus, x.digits, x.digits_size * sizeof(digit));
            free(x.digits);
            x.digits = temp;
            x.digits_size = temp_size;
        }

        res_exponent -= plus << 5;
        LongDouble res;
        int n = 1;
        while (x.digits_size > 2 * n || y.digits_size > n || x.digits_size - y.digits_size + 1 > n) n <<= 1;

        x.precision = INT_MAX;
        y.precision = INT_MAX;
        div21(x, y, n, res);

        res.precision = precision;
        res.exponent += res_exponent;
        res.sign = sign * other.sign;
        res.removeZeroes();
        if (res.digits_size > res.precision) {
            res.removeFirst(res.digits_size - res.precision);
        }
        res.removeZeroes();

        return res;
    }

    void LongDouble::operator/=(const LongDouble& other) { 
        *this = *this / LongDouble(other);
    }

};
