#pragma once

#define PRECISION_ADD
#define PRECISION_SUB
#define MIN_PRECISION 1

/*

base_exp - how mush digits in 10 base we save in one digit

input string format:
[-+][0-9][.][0-9]

output format:
use_scientific_output - scientific / not
outputing max presicion digits in given stream

how precision works
precison >= MIN_PRECISION
always use floor in math
add/sub works only if defines
for ex:
cout.precision() == 3
=> 
123 * 12345 = 15100000
division:
cout.precision() == 3
123 / 45 = 2.73

assymptotics:

sub / add O(n)
mul O(n * log n) (fft)
div O(n * log ^ 2 (n)) fast div algorithm
sqrt O(n * log ^ 2(n)) Karatsuba square root algorithm 

*/

namespace arithmetic {  

    using namespace std;
    using digit = int32_t;

    class LongDouble {
    private:
        int sign;
        digit* digits; 
        int digits_size;
        int exponent; // base 10
        int precision; // >= MIN_PRECISION

        void removeZeroes(); 
        void removeFirst(int value);

        template<class T>
        static void init_from_string(LongDouble&, const T&);
        template<class T>
        static void init_from_int(LongDouble&, const T&);
        template<class T>
        static void init_from_double(LongDouble&, const T&);

        static void div32(const LongDouble &a, const LongDouble &b, int n, LongDouble &res, LongDouble& rem);
        static void div21(const LongDouble &a, const LongDouble &b, int n, LongDouble &res);
        static void sqrt_rem(const LongDouble n, LongDouble &s, LongDouble &r);

    public:
        
        static bool use_scientific_output;
        static bool output_insignificant_zeroes;
        static const int base;
        static const int base_exp;
        static const int pow_10[10];
        static bool context_remove_left_zeroes;
        static int default_precision;

        LongDouble(); 
        LongDouble(const LongDouble& x); 
        LongDouble(const LongDouble& x, int p); 
        LongDouble(const char *value); 
        LongDouble(const char *value, int p);
        LongDouble(const string &value); 
        LongDouble(const string &value, int p);
        LongDouble(const int32_t &value);
        LongDouble(const int32_t &value, int p); 
        LongDouble(const int64_t &value);
        LongDouble(const int64_t &value, int p); 
        LongDouble(const uint64_t &value); 
        LongDouble(const uint64_t &value, int p);
        LongDouble(const double &value); 
        LongDouble(const double &value, int p); 
        LongDouble(const long double &value); 
        LongDouble(const long double &value, int p); 

        void set_precision(int); 
        int get_precision() const; 
        int get_sign() const;
        int get_digits_size() const;
        int get_exponent() const;

        bool isInt() const; 
        bool isZero() const; 
        bool isPower() const; 
        void mulBase(int power);
        void divBase(int power);
        void round(int number);
        void round();
        void floor(int number);
        void floor();
        void sqrt();
        void sqrt_int();
        void sqrt_fast();
        void pow(int power);
        LongDouble abs() const;
        LongDouble operator+(const LongDouble& x) const;
        void operator+=(const LongDouble& x);
        LongDouble operator-(const LongDouble& x) const;
        void operator-=(const LongDouble& x);
        LongDouble operator*(const LongDouble& x) const;
        void operator*=(const LongDouble& x);
        LongDouble operator/(const LongDouble& x) const;
        void operator/=(const LongDouble& x);

        LongDouble operator-() const;
        bool operator<(const LongDouble& x) const;
        bool operator<=(const LongDouble& x) const;
        bool operator>(const LongDouble& x) const;
        bool operator>=(const LongDouble& x) const;
        bool operator==(const LongDouble& x) const;
        bool operator!=(const LongDouble& x) const;
        LongDouble& operator=(const LongDouble& x);
        
        // LongDouble(LongDouble &&other) noexcept; // move
        // LongDouble& operator=(LongDouble &&other) noexcept; // move
        ~LongDouble();

        friend istream& operator>>(istream& os, LongDouble&);
        friend ostream& operator<<(ostream& os, const LongDouble&);

    };

    LongDouble operator""_ld (const char* x, unsigned long size);

    LongDouble operator""_ld (long double x);

    LongDouble operator""_ld (unsigned long long x);

    class DivisionByZeroException: public exception {
        const char * what() const noexcept override {
            return "Division by zero";
        }
    };

    class MemoryLimitException: public exception {
        const char * what() const noexcept override {
            return "Cannot allocate or reallocate memory";
        }
    };

    class InitError: public exception {
        const char * what() const noexcept override {
            return "Wrong format captured in initialization";
        }
    };

    class InitPrecisonError: public exception {
        const char * what() const noexcept override {
            return "Precision must be >= MIN_PRECISION constant";
        }
    };

    class InitStringError: public exception {
        const char * what() const noexcept override {
            return "Wrong format captured in string initialization";
        }
    };

    class SqrtLimitError: public exception {
        const char * what() const noexcept override {
            return "Expected non negative value";
        }
    };

    class SqrtIntLimitError: public exception {
        const char * what() const noexcept override {
            return "Expected integer >= 1";
        }
    };

    class NegativePowerError: public exception {
        const char * what() const noexcept override {
            return "Non negative value of power expected";
        }
    };

    class IntError: public exception {
        const char * what() const noexcept override {
            return "Integer expected";
        }
    };

    void division_error();

    void memory_error();

    void init_error();

    void init_precison_error();

    void init_string_error();

    void sqrt_limir_error();

    void sqrt_int_limir_error();

    void negative_power_error();

    void int_error();
    
};
