#include "decimal.h"
#include <cassert>

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
