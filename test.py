import cdecimal

a = cdecimal.Decimal('1.1')
b = cdecimal.Decimal('0.1')
c = cdecimal.Decimal('1.0')

assert(b + c == a)
print(a, b, c)
