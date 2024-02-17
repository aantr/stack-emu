#pragma once 

namespace fft {

    using namespace std;
    using digit = uint32_t;
    class fft { 
    public:
        fft();
        ~fft();
        void multiply(digit*& a, int size_a, digit*& b, int size_b, digit*& res, int &res_size, uint32_t base=10);
    };

}
