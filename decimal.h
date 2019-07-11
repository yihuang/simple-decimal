#include <cstdint>
#include <cstdlib>
#include <string>

class Decimal {
public:
    Decimal() : _n(0), _prec(0) {}
    Decimal(int64_t n, uint16_t prec);
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
    const std::string& to_string(std::string& result) const;
    bool set_string(const char* s) {
        return set_string(s, strlen(s));
    }
    bool set_string(const char* s, size_t len);

private:
    int64_t _n;
    uint16_t _prec;
};
