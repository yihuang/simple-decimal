#include "decimal.h"

#define PRECISION_LIMIT 19
#define MAX_BUF_DECIMAL 44
#define MAX_INT64_DIGITS (20+2)

static const uint64_t POW10[] = {
    1,
    10,
    100,
    1000,
    10000,
    100000,
    1000000,
    10000000,
    100000000,
    1000000000,
    10000000000,
    100000000000,
    1000000000000,
    10000000000000,
    100000000000000,
    1000000000000000,
    10000000000000000,
    100000000000000000,
    1000000000000000000,
    10000000000000000000U
};

Decimal::Decimal(int_type n, uint16_t prec) : _n(n) {
    assert(prec <= PRECISION_LIMIT);
    _prec = prec;
}

Decimal Decimal::operator +(const Decimal& other) const {
    if (_prec == other._prec) {
        return Decimal(_n + other._n, _prec);
    } else if (_prec > other._prec) {
        int_type n = other._n * POW10[_prec - other._prec];
        return Decimal(_n + n, _prec);
    } else {
        int_type n = _n * POW10[other._prec - _prec];
        return Decimal(n + other._n, other._prec);
    }
}

Decimal& Decimal::operator +=(const Decimal& other) {
    if (_prec == other._prec) {
        _n += other._n;
    } else if (_prec > other._prec) {
        _n += other._n * POW10[_prec - other._prec];
    } else {
        _n = _n * POW10[other._prec - _prec] + other._n;
        _prec = other._prec;
    }
    return *this;
}

Decimal Decimal::operator -(const Decimal& other) const {
    int_type n;
    if (_prec == other._prec) {
        return Decimal(_n - other._n, _prec);
    } else if (_prec > other._prec) {
        n = other._n * POW10[_prec - other._prec];
        return Decimal(_n - n, _prec);
    } else {
        n = _n * POW10[other._prec - _prec];
        return Decimal(n - other._n, other._prec);
    }
}

Decimal& Decimal::operator -=(const Decimal& other) {
    if (_prec == other._prec) {
        _n -= other._n;
    } else if (_prec > other._prec) {
        _n -= other._n * POW10[_prec - other._prec];
    } else {
        _n = _n * POW10[other._prec - _prec] - other._n;
        _prec = other._prec;
    }
    return *this;
}

Decimal Decimal::operator %(const Decimal& other) const {
    int_type n1 = _n;
    int_type n2 = other._n;
    uint16_t max_prec;
    if (_prec > other._prec) {
        max_prec = _prec;
        n2 *= POW10[_prec - other._prec];
    } else if (_prec < other._prec) {
        max_prec = other._prec;
        n1 *= POW10[other._prec - _prec];
    } else {
        max_prec = _prec;
    }
    return Decimal(n1 % n2, max_prec);
}

bool Decimal::operator ==(const Decimal& other) const {
    if (_prec == other._prec) {
        return _n == other._n;
    } else if (_prec > other._prec) {
        return _n == other._n * POW10[_prec - other._prec];
    } else {
        return _n * POW10[other._prec - _prec]  == other._n;
    }
}

bool Decimal::operator ==(uint64_t other) const {
    if (_prec == 0) {
        return _n == other;
    } else {
        return _n * POW10[_prec] == other;
    }
}

bool Decimal::operator <(const Decimal& other) const {
    if (_prec == other._prec) {
        return _n < other._n;
    } else if (_prec > other._prec) {
        return _n < other._n * POW10[_prec - other._prec];
    } else {
        return _n * POW10[other._prec - _prec] < other._n;
    }
}

void Decimal::normalize() {
    while (_prec != 0 && _n != 0 && _n % 10 == 0) {
        _n /= 10;
        _prec--;
    }
    if (_n == 0) {
        _prec = 0;
    }
}

const char* Decimal::to_string(char *buf, size_t size) const {
    char *p = buf+size-1;
    bool sign = _n >= 0;
    int_type n = _n >= 0 ? _n : -_n;
    size_t counter = 0;

    *(p--) = '\0';
    while (n != 0 || counter <= _prec) {
        if (counter++ == _prec) {
            if (_prec > 0) {
                *(p--) = '.';
                continue;
            }
        }
        if (n != 0) {
            *(p--) = '0' + (char)(n % 10);
            n /= 10;
        } else {
            *(p--) = '0';
        }
    }
    if (_prec > 0 && counter == _prec + 1) {
        *(p--) = '0';
    }
    if (!sign) {
        *(p--) = '-';
    }
    return p+1;
}

std::string Decimal::to_string() const {
    char buf[MAX_BUF_DECIMAL];
    const char* p = to_string(buf, sizeof(buf));
    return std::string(p, buf + sizeof(buf) - p - 1);
}

bool Decimal::set_string(const char* s, size_t len) {
    _n = 0;
    int i;
    int prec = 0;
    bool fractional = true;
    int digits = 0;
    for (i=(int)len-1; i>=0; i--) {
        if (s[i] == '.') {
            fractional = false;
        } else {
            if (s[i] > '0' && s[i] <= '9') {
                _n += (s[i] - '0') * POW10[digits];
            } else if (s[i] != '0') {
                break;
            }
            digits++;
            if (fractional) {
                prec++;
            }
        }
    }
    _prec = !fractional ? prec : 0;
    if (i >= 0) {
        if (s[i] == '-') {
            _n = -_n;
        } else if (s[i] != '+') {
            return false;
        }
    }

    return true;
}

uint128_t multiple(uint64_t a, unsigned a_exp, uint64_t b, unsigned b_exp, unsigned *o_exp) {
    *o_exp = a_exp + b_exp;
    return (uint128_t)a * (uint128_t)b;
}

#define DIGITS_LIMIT_64BIT 20
bool parse_decimal(const char* s, size_t len, unsigned exp_limit, uint64_t* _n) {
    // parse unsigned decimal with exp_limit scale.
    uint64_t n = 0;
    int i;
    int exp = 0;
    bool met_dot = false;  // had we met dot?
    int digits = 0;

    if (len > MAX_INT64_DIGITS) {
        // overflow
        return false;
    }
    if (len > 0 && s[0] == '-') {
        return false;
    }
    for (i=(int)len-1; i>=0; i--) {
        if (s[i] == '.') {
            met_dot = true;
        } else {
            if (s[i] > '0' && s[i] <= '9') {
                n += (s[i] - '0') * POW10[digits];
            } else if (s[i] != '0') {
                // invalid character
                break;
            }
            digits++;
            if (digits == DIGITS_LIMIT_64BIT) {
                // may overflow
                return false;
            }
            if (!met_dot) {
                exp++;
            }
        }
    }
    if (i >= 0 && s[i] != '+') {
        // invalid character
        return false;
    }

    exp = met_dot ? exp : 0;
    if (exp > exp_limit) {
        return false;
    } else {
        *_n = n * POW10[exp_limit - exp];
    }
    return true;
}

bool parse_decimal(const char* s, size_t len, unsigned *_exp, uint64_t* _n) {
    // parse unsigned decimal
    uint64_t n = 0;
    int i;
    int exp = 0;
    bool met_dot = false;  // had we met dot?
    int digits = 0;

    if (len > MAX_INT64_DIGITS) {
        // overflow
        return false;
    }
    if (len > 0 && s[0] == '-') {
        return false;
    }
    for (i=(int)len-1; i>=0; i--) {
        if (s[i] == '.') {
            met_dot = true;
        } else {
            if (s[i] > '0' && s[i] <= '9') {
                n += (s[i] - '0') * POW10[digits];
            } else if (s[i] != '0') {
                // invalid character
                break;
            }
            digits++;
            if (digits == DIGITS_LIMIT_64BIT) {
                // may overflow
                return false;
            }
            if (!met_dot) {
                exp++;
            }
        }
    }
    if (i >= 0 && s[i] != '+') {
        // invalid character
        return false;
    }

    exp = met_dot ? exp : 0;
    *_exp = exp;
    *_n = n;
    return true;
}
