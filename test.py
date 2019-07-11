import cdecimal
import decimal
from hypothesis import given
from hypothesis.strategies import floats


@given(floats(), floats())
def test_add(f1, f2):
    s1, s2 = str(f1), str(f2)
    cd1, cd2 = cdecimal.Decimal(s1), cdecimal.Decimal(s2)
    d1, d2 = decimal.Decimal(s1), decimal.Decimal(s2)
    assert str(cd1) == s1
    assert str(cd2) == s2
    assert str(cd1 + cd2) == str(d1 + d2)


if __name__ == '__main__':
    test_add()
