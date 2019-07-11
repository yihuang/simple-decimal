Simple Decimal data type in C++
===============================

I can't find an easy to use decimal library for c++, so I write my own.

It uses an ``int128_t`` to store digits, and an extra field to store 10 based exponents.
