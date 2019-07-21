#include <cstdint>
#include <cstdlib>
#include <string>
#include "int128.h"

uint128_t multiple(uint64_t a, unsigned a_exp, uint64_t b, unsigned b_exp, unsigned *o_exp);
bool parse_decimal(const char* s, size_t len, unsigned exp_limit, uint64_t* _n);
bool parse_decimal(const char* s, size_t len, unsigned *_exp, uint64_t* _n);

template<typename int_type>
char* format_decimal(char *buf, size_t size, int_type _n, unsigned exp) {
    char *p = buf+size-1;  // work from end
    bool sign = _n >= 0;
    bool remove_trailing_zero = exp > 0;
    int_type n = sign ? _n : -_n;
    size_t counter = 0;

    *(p--) = '\0';
    if (n == 0) {
        *(p--) = '0';
        return p+1;
    }

    while (n != 0 || counter <= exp) {
        if (counter++ == exp) {
            if (exp > 0) {
                if (!remove_trailing_zero) {
                    *(p--) = '.';
                }
                remove_trailing_zero = false;
                continue;
            }
        }
        if (n != 0) {
            unsigned char c = (n % 10);
            if (c == 0 && remove_trailing_zero) {
            } else {
                remove_trailing_zero = false;
                *(p--) = '0' + c;
            }
            n /= 10;
        } else {
            *(p--) = '0';
        }
    }
    if (exp > 0 && counter == exp + 1) {
        *(p--) = '0';
    }
    if (!sign) {
        *(p--) = '-';
    }
    return p+1;
}

