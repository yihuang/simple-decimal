#include <cstdint>
#include <cstdlib>
#include <string>
#include "int128.h"

typedef int128_t int_type;

class Decimal {
public:
    Decimal() : _n(0), _prec(0) {}
    Decimal(int_type n, uint16_t prec);
    Decimal(const std::string& s) {
        set_string(s.c_str(), s.size());
    }
    Decimal(const char* p, size_t size) {
        set_string(p, size);
    }

    Decimal neg() const {
        return Decimal(-_n, _prec);
    }

    Decimal operator +(const Decimal& other) const;
    Decimal& operator +=(const Decimal& other);
    Decimal operator -(const Decimal& other) const;
    Decimal& operator -=(const Decimal& other);

    Decimal operator *(const Decimal& other) const {
        return Decimal(_n * other._n, _prec + other._prec);
    }

    Decimal operator /(const Decimal& other) const {
        return Decimal(_n / other._n, other._prec - _prec);
    }

    Decimal operator %(const Decimal& other) const;
    bool operator ==(const Decimal& other) const;
    bool operator ==(uint64_t other) const;

    bool operator !=(const Decimal& other) const {
        return !(*this == other);
    }

    bool operator !=(uint64_t other) const {
        return !(*this == other);
    }

    bool operator <(const Decimal& other) const;
    bool operator <=(const Decimal& other) const {
        return *this==other || *this < other;
    }

    bool operator >(const Decimal& other) const {
        return !(*this <= other);
    }

    void normalize();

    bool is_normalized() const {
        return _prec == 0 || _n % 10 != 0;
    }

    bool is_zero() const {
        return _n == 0;
    }

    int sign() {
        return _n < 0 ? -1 : (_n > 0 ? 1 : 0);
    }

    const char* to_string(char *buf, size_t size) const;
    std::string to_string() const;
    bool set_string(const char* s) {
        return set_string(s, strlen(s));
    }
    bool set_string(const char* s, size_t len);

    int_type _n;
    uint16_t _prec;
};

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

