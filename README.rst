Simple Decimal data type in C++
===============================

I can't find an easy to use decimal library for c++, so I write my own.

It uses an `int128_t <https://github.com/zhanhb/int128.git>`_ to store digits, and an extra field to store 10 based exponents.

The max precision is limited to 19, because that's the max pow of ten an ``uint64_`` can hold, and it's enough for my own usage.

Also you need to control the number limits yourself, because even if it's 128bit integer, overflow can still happen.
