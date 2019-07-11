#include "decimal.h"
#define PRECISION_LIMIT 19

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

Decimal::Decimal(int64_t n, uint16_t prec) : _n(n) {
    assert(prec < PRECISION_LIMIT);
    _prec = prec;
}

Decimal Decimal::operator +(const Decimal& other) const {
    if (_prec == other._prec) {
        return Decimal(_n + other._n, _prec);
    } else if (_prec > other._prec) {
        int64_t n = other._n * POW10[_prec - other._prec];
        return Decimal(_n + n, _prec);
    } else {
        int64_t n = _n * POW10[other._prec - _prec];
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
    int64_t n;
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
    uint64_t n1 = _n;
    uint64_t n2 = other._n;
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
    int64_t n = abs(_n);
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
            *(p--) = '0' + n % 10;
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

const std::string& Decimal::to_string(std::string& result) const {
    char buf[32];
    const char* p = to_string(buf, sizeof(buf));
    result.clear();
    result.append(p, buf + sizeof(buf) - p - 1);
    return result;
}

bool Decimal::set_string(const char* s, size_t len) {
    _n = 0;
    int prec = 0;
    // _sign = s[0] != '-';
    //bool stripzero = true;
    bool fractional = true;
    int digits = 0;
    for (int i=(int)len-1; i>=0; i--) {
        if (s[i] == '.') {
            fractional = false;
        } else {
            if (s[i] >= '0' && s[i] <= '9') {
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
    return true;
}
