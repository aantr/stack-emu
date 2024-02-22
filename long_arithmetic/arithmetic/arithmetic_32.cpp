#include <defines.hpp>

#include <iostream>
#include <cstring>
#include <string>
#include <assert.h>
#include <iomanip>
#include <exception>
#include <arithmetic_32.hpp>
#include <fft.hpp>
#include <math.h>
#include <climits>

namespace arithmetic_32 {

    using namespace std;

    bool LongDouble::context_remove_left_zeroes = true;
    bool LongDouble::output_insignificant_zeroes = false;
    int LongDouble::default_precision = 8;
    const uint64_t LongDouble::base = 1ll << 32;
    digit LongDouble::ones[33] = {  0b0u, 0b1u, 0b11u, 0b111u, 0b1111u, 0b11111u, 0b111111u, 
                                    0b1111111u, 0b11111111u, 0b111111111u, 0b1111111111u, 
                                    0b11111111111u, 0b111111111111u, 0b1111111111111u, 
                                    0b11111111111111u, 0b111111111111111u, 
                                    0b1111111111111111u, 0b11111111111111111u, 
                                    0b111111111111111111u, 0b1111111111111111111u, 
                                    0b11111111111111111111u, 0b111111111111111111111u, 
                                    0b1111111111111111111111u, 0b11111111111111111111111u, 
                                    0b111111111111111111111111u, 0b1111111111111111111111111u, 
                                    0b11111111111111111111111111u, 0b111111111111111111111111111u, 
                                    0b1111111111111111111111111111u, 0b11111111111111111111111111111u, 
                                    0b111111111111111111111111111111u, 0b1111111111111111111111111111111u, 
                                    0b11111111111111111111111111111111u };


    void division_error() {
        throw DivisionByZeroException();
    }   

    void memory_error() {
        throw MemoryLimitException();
    }

    void init_error() {
        throw InitError();
    }

    void init_precison_error() {
        throw InitPrecisonError();
    }

    void init_string_error() {
        throw InitStringError();
    }

    void sqrt_limit_error() {
        throw SqrtLimitError();
    }

    void sqrt_int_limit_error() {
        throw SqrtIntLimitError();
    }

    void negative_power_error() {
        throw NegativePowerError();
    }

    void int_error() {
        throw InitError();
    }

    LongDouble::LongDouble(LongDouble &&other) noexcept { // move;
        sign = exchange(other.sign, 1);
        digits = exchange(other.digits, nullptr);
        digits_size = exchange(other.digits_size, 0);
        precision = exchange(other.precision, default_precision);
        exponent = exchange(other.exponent, 0);
    }

    LongDouble::~LongDouble() {
        free(digits);
    }

    LongDouble::LongDouble() {
        digits = (digit*) malloc(0);
        sign = 1;
        digits_size = 0;
        exponent = 0;
        precision = default_precision;
    }

    LongDouble::LongDouble(const LongDouble& x) {
        sign = x.sign;
        digits_size = x.digits_size;
        digits = (digit*) malloc(x.digits_size * sizeof(digit));
        if (!digits) memory_error();
        memcpy(digits, x.digits, x.digits_size * sizeof(digit));
        assert(digits != x.digits);
        precision = x.precision;
        exponent = x.exponent;
    }

    LongDouble::LongDouble(const LongDouble& x, int p) {
        if (p < MIN_PRECISION) {
            init_precison_error();
        }
        sign = x.sign;
        digits_size = x.digits_size;
        digits = (digit*) malloc(x.digits_size * sizeof(digit));
        if (!digits) memory_error();
        memcpy(digits, x.digits, x.digits_size * sizeof(digit));
        assert(digits != x.digits);
        exponent = x.exponent;
        precision = p;
    }

    template<class T>
    void LongDouble::init_from_int(LongDouble& res, const T& value) { // todo: check restrictions
        T x = value;
        res.sign = 1;
        if (x < 0) res.sign = -1, x = -x;
        res.digits_size = 0;
        res.digits = (digit*) malloc(res.digits_size * sizeof(digit));
        if (!res.digits) memory_error();
        while (x) {
            res.digits = (digit*) realloc(res.digits, (res.digits_size + 1) * sizeof(digit));
            if (!res.digits) memory_error();

            res.digits[res.digits_size] = x & LongDouble::ones[32];
            // cout << "digit: " <<  res.digits[res.digits_size]  << endl;;
            res.digits_size++;
            x /= res.base;
        }
        res.exponent = 0;
        res.removeZeroes();
    }

    template<class T>
    void LongDouble::init_from_double(LongDouble& res, const T& v) {
        int exp;
        int sign = 1;
        if (v < 0) {
            sign = -1;
        }
        double fr = frexp(std::abs(v), &exp);
        fr *= 1ll << 62;
        int64_t result = std::round(fr);
        init_from_int(res, result * sign);
        // cout << "constructor: " << result << " " << exp - 62 << endl;
        res.exponent += exp - 62;
    }

    LongDouble::LongDouble(const int32_t &v) {
        LongDouble::init_from_int(*this, v);
        precision = default_precision;
    }

    LongDouble::LongDouble(const int32_t &v, int p): precision(p) {
        if (p < MIN_PRECISION) {
            init_precison_error();
        }
        LongDouble::init_from_int(*this, v);
    }

    LongDouble::LongDouble(const int64_t &v) {
        LongDouble::init_from_int(*this, v);
        precision = default_precision;
    }

    LongDouble::LongDouble(const int64_t &v, int p): precision(p) {
        if (p < MIN_PRECISION) {
            init_precison_error();
        }
        LongDouble::init_from_int(*this, v);
    }

    LongDouble::LongDouble(const uint64_t &v) {
        LongDouble::init_from_int(*this, v);
        precision = default_precision;
    }

    LongDouble::LongDouble(const uint64_t &v, int p): precision(p) {
        if (p < MIN_PRECISION) {
            init_precison_error();
        }
        LongDouble::init_from_int(*this, v);
    }

    LongDouble::LongDouble(const double &v) {
        LongDouble::init_from_double(*this, v);
        precision = default_precision;
    }

    LongDouble::LongDouble(const double &v, int p): precision(p) {
        if (p < MIN_PRECISION) {
            init_precison_error();
        }
        LongDouble::init_from_double(*this, v);
    }

    LongDouble::LongDouble(const long double &v) {
        LongDouble::init_from_double(*this, v);
        precision = default_precision;
    }

    LongDouble::LongDouble(const long double &v, int p): precision(p) {
        if (p < MIN_PRECISION) {
            init_precison_error();
        }
        LongDouble::init_from_double(*this, v);
    }

    void LongDouble::set_precision(int v) {
        if (v < MIN_PRECISION) {
            init_precison_error();
        }
        precision = v;
    } 
    
    int LongDouble::get_precision() const {
        return precision;
    }

    int LongDouble::get_sign() const {
        return sign;
    }

    int LongDouble::get_digits_size() const {
        return digits_size;
    }

    int LongDouble::get_exponent() const {
        return exponent;
    }

    LongDouble LongDouble::abs() const {
        if (digits_size == 0) return 0;
        if (sign == -1) return -(*this);
        return *this;
    }

    bool LongDouble::isInt() const {
        return exponent >= 0;
    }

    bool LongDouble::isZero() const {
        return digits_size == 0;
    }

    bool LongDouble::isPower() const {
        return digits_size == 1 && digits[0] == 1;
    }

    /*
    Removes all null right bits and changes exponent
    */
    void LongDouble::removeRightZeroes() {
        removeZeroes();
        if (digits_size == 0) {
            return;
        }
        int d = 0;
        while ((digits[0] >> d & 1) == 0) {
            d++;
        }
        if (d == 0) {
            return;
        }
        for (int i = 0; i < digits_size; i++) {
            if (i) {
                digits[i - 1] |= (digits[i] & (ones[d])) << (32 - d);
            }
            digits[i] = digits[i] >> d;
        }
        exponent += d;
        removeZeroes();
    }

    /*
    Removes all null left and right -digits- and changes exponent
    Does not remove left digits if context_remove_left_zeroes == false
    */
    void LongDouble::removeZeroes() {
        int left = 0, right = 0;
        for (int i = digits_size - 1; i >= 0; i--) {
            if (digits[i] == 0) {
                right++;
            } else {
                break;
            }
        }
        if (context_remove_left_zeroes) {
            for (int i = 0; i < digits_size; i++) {
                if (digits[i] == 0) {
                    exponent += 32;
                    left++;
                } else {
                    break;
                }
            }
        }
        if (right == digits_size) {
            free(digits);
            digits = (digit*) malloc(0 * sizeof(digit));
            if (!digits) memory_error();
            digits_size = 0;
            sign = 1;
            exponent = 0;
        } else {
            if (left == 0) {
                if (right == 0) {
                    return;
                }
                digits_size = digits_size - right;
                digits = (digit*) realloc(digits, (digits_size) * sizeof(digit));
                if (!digits) memory_error();

            } else {
                digits_size = digits_size - left - right;
                digit* temp = (digit*) malloc(digits_size * sizeof(digit));
                if (!temp) memory_error();
                memcpy(temp, digits + left, digits_size * sizeof(digit));
                free(digits);
                digits = temp;
            }
        }
        
    }

    /*
    Returns new obj that consists of first value bits
    */
    LongDouble LongDouble::getFirstBits(unsigned int value) const {
        assert(isInt());
        assert((unsigned int) (digits_size << 5) + exponent >= value);
        unsigned int e = min(value, (unsigned int) exponent);
        value -= e;
        LongDouble res(0, precision);
        if (value) {
            res.exponent = e;
            free(res.digits);
            res.digits_size = ((value - 1) >> 5) + 1;
            res.digits = (digit*) malloc(res.digits_size * sizeof(digit));
            if (!res.digits) memory_error();
            memcpy(res.digits, digits, res.digits_size * sizeof(digit));
            int remove = (res.digits_size << 5) - value;
            if (remove) {
                res.digits[res.digits_size - 1] &= ((1u << (32 - remove)) - 1);
            }
        }
        res.removeZeroes();
        return res;
    }

    /*
    Removes first value digits from right sight 
    */
    void LongDouble::removeFirst(int value) {
        assert(digits_size >= value && value >= 0);
        digits_size = digits_size - value;
        digit* temp = (digit*) malloc(digits_size * sizeof(digit));
        if (!temp) memory_error();
        memcpy(temp, digits + value, digits_size * sizeof(digit));
        free(digits);
        digits = temp;
        exponent += value << 5;
    }

    void LongDouble::mulBase(int power) { // base = 2
        exponent += power;
    }

    void LongDouble::divBase(int power) { // base = 2
        exponent -= power;
    }

    /*
    Round using math rules and 2 base
    */
    void LongDouble::round(int number) { // base = 2
        if (-exponent <= number || digits_size == 0) return;
        if (-exponent - number - 1 >= digits_size << 5) {
            *this = LongDouble(0, precision);
            return;
        }
        int d = (-exponent - number - 1) >> 5;
        removeFirst(d);
        assert(digits_size >= 1);
        int rem = (-exponent - number - 1) & (31);
        digits[0] -= digits[0] & ones[rem];

        bool was = true;
        if ((digits[0] >> rem & 1) < 1) was = false;
        if (was) {
            LongDouble st(sign);
            st.divBase(number);
            *this += st;
        }

        d = (-exponent - number - 1) >> 5;
        rem = (-exponent - number - 1) & (31);
        // delete rem-th digit
        digits[d] -= digits[d] & ones[rem + 1];
        removeZeroes();

    }

    void LongDouble::round() {
        round(0);
    }

    /*
    Floor using 2 base
    */
    void LongDouble::floor(int number) {
        if (-exponent <= number || digits_size == 0) return;
        if (-exponent - number >= digits_size << 5) {
            *this = LongDouble(0, precision);
            return;
        }
        removeFirst((-exponent - number) >> 5);
        int rem = (-exponent - number) & (31);
        if (rem > 0) {
            digits[0] -= digits[0] & ones[rem];
        }
        removeZeroes();
    }

    void LongDouble::floor() {
        floor(0);
    }

    /*
    Simple sqrt using binary search
    */
    void LongDouble::sqrt() { // use binary search
        if (*this < 0) {
            sqrt_limit_error(); 
        }
        if (*this == 0) {
            return;
        }
        LongDouble l, r = *this + 1;
        l.precision = precision;
        r.precision = precision;
        LongDouble prev = -1;
        int less_one =  (*this < 1);
        if (less_one) {
            r = LongDouble(1, precision);
        }
        while (1) {
            LongDouble m = (l + r) * 0.5;
            if (m.digits_size > m.precision) {
                m.removeFirst(m.digits_size - m.precision); // floor m
            }
            if (m * m <= *this) {
                l = m;
            } else {                
                r = m;
            }
            if (prev == m) break;
            prev = m;
        }
        if (l.digits_size > l.precision) l.removeFirst(l.digits_size - l.precision);
        *this = l;
    }

    /*
    Fast sqrt
    */
    void LongDouble::sqrt_fast() { // works only for integers >= 1
        if (!(isInt() && *this >= 1)) {
            sqrt_int_limit_error();
        }
        int plus = max(0, precision - (digits_size - (exponent >> 5)) / 2 + 1);
        mulBase((plus * 2) << 5);
        LongDouble s, r;
        sqrt_rem(LongDouble(*this, digits_size + (exponent >> 5) + 1), s, r);

        assert(*this == s * s + r);

        *this = LongDouble(s, precision);
        divBase(plus << 5);

        if (digits_size > precision) {
            removeFirst(digits_size - precision);
        }
    }

    void LongDouble::pow(int power) {
        if (power < 0) {
            negative_power_error();
        }
        if (power == 0) {
            *this = LongDouble(1, precision);
            return;
        }
        LongDouble res = *this;
        res.pow(power / 2);
        if (power & 1) {
            *this = res * res * (*this);
        } else {
            *this = res * res;
        }
    }

    /*
    Input: const a, b, st: 
        exponent == 0, sign == 0, precision == INT_MAX, 
        the highest digit of a, b is not null,
        |a| <= 2n, |b| <= n, |result| <= n!
    Output: result of division floor(a / b)
    */
    void LongDouble::div21(const LongDouble &a, const LongDouble &b, int n, LongDouble &res) {
        assert(a.exponent == 0 && b.exponent == 0 && a.sign == 1 && b.sign == 1);
        assert(a.precision == INT_MAX && b.precision == INT_MAX);
        assert(a.isZero() || a.digits[a.digits_size - 1] != 0); 
        assert(b.isZero() || b.digits[b.digits_size - 1] != 0);
        // should be assert on no right nulls digits

        LongDouble x = a, y = b;
        x.precision = INT_MAX;
        y.precision = INT_MAX;

        int m = n >> 1;
        if (n <= 1) {
            uint64_t A = 0, B = 0;
            for (int i = x.digits_size - 1; i >= 0; i--) {
                A = (A << 32) | x.digits[i]; // changed
            }
            for (int i = y.digits_size - 1; i >= 0; i--) {
                B = (B << 32) | y.digits[i]; //changed
            }
            if (B == 0) {
                division_error();
            }
            res = LongDouble(A / B, INT_MAX);
            return;
        }

        // div32 first n - m digits by y
        LongDouble first3m(0, INT_MAX);

        if (x.digits_size >= m) {
            first3m.digits_size = x.digits_size - m;
            free(first3m.digits);
            first3m.digits = (digit*) malloc(first3m.digits_size * sizeof(digit));
            if (!first3m.digits) memory_error();
            memcpy(first3m.digits, x.digits + m, (first3m.digits_size) * sizeof(digit));
            LongDouble::context_remove_left_zeroes = false;
            first3m.removeZeroes();
            LongDouble::context_remove_left_zeroes = true;
        } else {
            free(first3m.digits);
            first3m.digits = (digit*) malloc(0 * sizeof(digit));
            if (!first3m.digits) memory_error();
        }

        LongDouble rem1(0, INT_MAX);
        res = LongDouble(0, INT_MAX);

        div32(first3m, y, m, res, rem1);

        // add m last x digits to result and divide by y
        rem1.digits_size = rem1.digits_size + m;
        digit* temp = (digit*) malloc(rem1.digits_size * sizeof(digit));
        if (!temp) memory_error();
        memset(temp, 0, m * sizeof(digit));

        memcpy(temp + m, rem1.digits, (rem1.digits_size - m) * sizeof(digit));
        free(rem1.digits);
        rem1.digits = temp;
        memcpy(rem1.digits, x.digits, min(m, x.digits_size) * sizeof(digit));
        LongDouble res2(0, INT_MAX), rem2(0, INT_MAX);

        LongDouble::context_remove_left_zeroes = false;
        rem1.removeZeroes();
        LongDouble::context_remove_left_zeroes = true;

        div32(rem1, y, m, res2, rem2);

        // make res = (res << m) | res2
        res.digits_size = res.digits_size + m;
        temp = (digit*) malloc(res.digits_size * sizeof(digit));
        if (!temp) memory_error();
        memset(temp, 0, m * sizeof(digit));

        memcpy(temp + m, res.digits, (res.digits_size - m) * sizeof(digit));

        free(res.digits);
        res.digits = temp;

        memcpy(res.digits, res2.digits, res2.digits_size * sizeof(digit));
        LongDouble::context_remove_left_zeroes = false;
        res.removeZeroes();
        LongDouble::context_remove_left_zeroes = true;

        LongDouble rem = a - b * res;
        assert(a >= b * res);
        assert(a < b * (res + 1));
        assert(rem < b);
        assert(rem >= 0);
    }

    /*
    Input: const a, b, st: 
        exponent == 0, sign == 0, precision == INT_MAX, 
        the highest digit of a, b is not null,
        |a| <= 3n, |b| <= 2n, |result| <= n!
    Output: result of division floor(a / b), reminder = a - b * result. precison = INT_MAX
    */
    void LongDouble::div32(const LongDouble &a, const LongDouble &b, int n, LongDouble &res, LongDouble& rem) {
        assert(a.exponent == 0 && b.exponent == 0 && a.sign == 1 && b.sign == 1);
        assert(a.isZero() || a.digits[a.digits_size - 1] != 0);
        assert(b.isZero() || b.digits[b.digits_size - 1] != 0);
        assert(a.precision == INT_MAX && b.precision == INT_MAX);

        LongDouble x = a, y = b;
        x.precision = INT_MAX;
        y.precision = INT_MAX;

        if (n <= 1) {

            __uint128_t A = 0, B = 0;
            for (int i = x.digits_size - 1; i >= 0; i--) { // 32 * 3
                A = (A << 32) | x.digits[i];  // changed
            }
            for (int i = y.digits_size - 1; i >= 0; i--) { 
                B = (B << 32) | y.digits[i]; // changed
            }
            if (B == 0) {
                division_error();
            }
            // cout << A << " " << B << " " << res << " " << rem << endl;
            res = LongDouble((uint64_t) (A / B), INT_MAX);
            rem = LongDouble((uint64_t) (A % B), INT_MAX);
            // cout << x << " "<<y << " " << res << " " << rem << endl;


            assert((rem.exponent % 32) == 0);
            int d = rem.exponent >> 5;
            rem.digits_size += d;
            digit* temp = (digit*) malloc(rem.digits_size * sizeof(digit));
            if (!temp) memory_error();
            memset(temp, 0, d * sizeof(digit));

            memcpy(temp + d, rem.digits, (rem.digits_size - d) * sizeof(digit));
            free(rem.digits);
            rem.digits = temp;
            rem.exponent = 0;
            assert(a == b * res + rem);
            assert(rem >= 0);
            assert(rem < b);

            return;
        }

        if (y.digits_size <= n) {
            div21(x, y, n, res);
            LongDouble::context_remove_left_zeroes = false;
            rem = x - y * res;
            LongDouble::context_remove_left_zeroes = true;
            assert(rem >= 0);
            assert(rem.exponent == 0);
            assert(rem.sign == 1);
            assert(a == b * res + rem);
            assert(rem < b);
            assert(rem >= 0);
            return;
        }

        LongDouble x1(0, INT_MAX); // first |x| - (|y| - n)
        x1.digits_size = max(0, x.digits_size - (y.digits_size - n));
        free(x1.digits);
        x1.digits = (digit*) malloc(x1.digits_size * sizeof(digit));
        if (!x1.digits) memory_error();

        memcpy(x1.digits, x.digits + (y.digits_size - n), (x1.digits_size) * sizeof(digit));

        LongDouble y1(0, INT_MAX); // first n
        y1.digits_size = n;
        free(y1.digits);
        y1.digits = (digit*) malloc(y1.digits_size * sizeof(digit));
        if (!y1.digits) memory_error();

        memcpy(y1.digits, y.digits + (y.digits_size - n), (y1.digits_size) * sizeof(digit));

        // y *= base ^ n
        y1.digits_size += n;
        digit* temp = (digit*) malloc(y1.digits_size * sizeof(digit));
        if (!temp) memory_error();
        memset(temp, 0, n * sizeof(digit));

        memcpy(temp + n, y1.digits, (y1.digits_size - n) * sizeof(digit));
        free(y1.digits);
        y1.digits = temp;

        bool maxres = x1 >= y1;

        // y /= base ^ n
        y1.digits_size -= n;
        temp = (digit*) malloc(y1.digits_size * sizeof(digit));
        if (!temp) memory_error();

        memcpy(temp, y1.digits + n, y1.digits_size * sizeof(digit));
        free(y1.digits);
        y1.digits = temp;

        // if x1 >= y1 * base ^ n dont call div21

        // some statements
        // x1/y1 >= x/y
        // x1 / (y1 + 1) <= x/y
        // x1/y1 - base <= x1 / (y1 + 1)
        // x1 / (y1 + 1) <= x/y <= x1/y1
        // x1/y1-base <= x/y <= x1/y1

        res = LongDouble(0, INT_MAX);
        if (maxres) {
            res = 1;
            res.precision = INT_MAX;
            res.mulBase(n << 5);
            res -= 1;
        } else {
            assert(x1.isZero() || x1.digits[x1.digits_size - 1] != 0);
            div21(x1, y1, n, res);            
        }

        LongDouble::context_remove_left_zeroes = false;

        LongDouble current_rem = LongDouble(x, INT_MAX) - res * y; 
        assert(current_rem.isZero() || current_rem.digits[current_rem.digits_size - 1] != 0);
        assert(y.isZero() || y.digits[y.digits_size - 1] != 0);
        assert(current_rem.isZero() || current_rem.digits[current_rem.digits_size - 1] != 0);

        uint64_t q;

        uint64_t A = 0, B = 0;
        if (current_rem.digits_size < y.digits_size || current_rem >= 0) {
            q = 0;
        } else {
            for (int i = 0; i < min(2, current_rem.digits_size); i++) {
                A = (A << 32) | current_rem.digits[current_rem.digits_size - 1 - i];
                B = (B << 32) | y.digits[y.digits_size - 1 - i];
            }
            if (A == 0) {
                q = 0;
            } else {
                q = A / B;
            }
        }

        assert(q == 0 || current_rem.digits_size < y.digits_size || current_rem.digits_size - y.digits_size == 0);
        assert(q == 0 || current_rem.digits_size < y.digits_size || (q >= A / B && q - A / B <= 1));

        current_rem += y * q;
        res -= q;  

        assert(q <= (1ll << 32));

        while (current_rem < 0) {
            res -= 1;
            current_rem += y;
        }

        LongDouble::context_remove_left_zeroes = true;
        assert(res.exponent == 0);
        rem = current_rem;

        assert(a == b * res + rem);
        assert(rem < b);
        assert(rem >= 0);
    }

    /*
    Input: const n, st: 
        n is integer, n >= 1, the highest digit is not null
    Output: s = floor(sqrt(n)), reminder = n - s * s, precison = INT_MAX
    */
    void LongDouble::sqrt_rem(const LongDouble n, LongDouble &s, LongDouble &r) {


        if (!(n.isInt() && n >= 1)) {
            sqrt_int_limit_error();
        }

        assert(n.isZero() || n.digits[n.digits_size - 1] != 0);


        if (n <= 4) {
            if (n == 1) {
                s = 1;
                r = 0;
            } else if (n == 2) {
                s = 1;
                r = 1;
            } else if (n == 3) {
                s = 1;
                r = 2;
            } else {
                s = 2;
                r = 0;
            }
            assert(n == s * s + r);

            return;
        }

        LongDouble x(n, INT_MAX);

        int power = (x.digits_size << 5) + x.exponent;
        int left = 0;
        while ((x.digits[x.digits_size - 1] >> (31 - left) & 1) == 0) {
            left++;
            power--;
        }

        int was = 0;
        int rem = power & 3;
        if (rem == 1 || rem == 2) {
            x <<= 2;
            was = 1;
        }

        int power_b = ((power - 1) >> 2) + 1;
        LongDouble b = 1_ld << power_b;
        LongDouble a0 = x.getFirstBits(power_b);
        x >>= power_b;
        LongDouble a1 = x.getFirstBits(power_b);
        x >>= power_b;
        LongDouble a2 = x.getFirstBits(power_b);
        x >>= power_b;

        LongDouble a3 = x;

        LongDouble s1, r1, q, u;
        sqrt_rem((a3 << power_b) + a2, s1, r1);
        assert((a3 << power_b) + a2 == s1 * s1 + r1);
        assert(r1 >= 0);
        assert((a3 << power_b) + a2 < (s1 + 1) * (s1 + 1));

        s1.precision = INT_MAX;
        r1.precision = INT_MAX;

        LongDouble A((r1 << power_b) + a1, INT_MAX), B((s1 << 1), INT_MAX);

        A.precision = max(MIN_PRECISION, A.digits_size + ((A.exponent - 1) >> 5) - (B.digits_size + ((B.exponent - 1) >> 5))) + 3;

        q = A / B;
        q.floor();
        q.precision = INT_MAX;
        u = A - q * B;
        u.precision = INT_MAX;
        s = (s1 << power_b) + q;
        r = (u << power_b) + a0 - q * q;

        assert(A >= B * q);
        assert(A < B * (q + 1) * (q + 1));
        assert(A == B * q + u);

        assert(s == s1 * b + q);
        assert(u == (r + q * q - a0) >> power_b);
        assert(r == u * b + a0 - q * q);
        assert((n << (was ? 2 : 0)) == s * s + r);


        if (r < 0) {
            r = r + (s << 1) - 1;
            s -= 1;
        }

        assert((n << (was ? 2 : 0)) == s * s + r);

        if (was) {
            s >>= 1;
            r = n - s * s;
            was = 0;
        }
        assert(n == s * s + r);
        assert(n >= s * s);
        assert(n < (s + 1) * (s + 1));


    }

};
