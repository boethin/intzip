# IntZip
Fast compression of integer sets.

[![Build Status](https://travis-ci.org/boethin/intzip.svg?branch=master)](https://travis-ci.org/boethin/intzip)

* [Description](#description)
* [Efficiency Considerations](#efficiency-considerations)
* [C++ Implementation](#c-implementation)
  * [Encoding](#encoding)
  * [Decoding](#decoding)
  * [Containment Testing](#containment-testing)
  * [Specializations](#specializations)
* [Sample Usage](#sample-usage)

## Description

_IntZip_ is a lossless compression algorithm for strictly increasing lists of unsigned integers.

Compression means that a suitable list of integers can be encoded to a (whenever possible) shorter list of the same integer format. An arbitrary set of integers (i.e., a collection of unique numbers where the order does not matter) may be transformed into suitable input data by way of translation and sorting.

The main focus of the algorithm is to encode structural characteristics of a given list, such as coherent intervals and repeated distances. Therefore, the quality of compression depends on diversity rather than size. In most cases, _IntZip_ is able to achieve much better compression results than general compression algorithms on such data sets. For example:

* The complete list of all 65536 16-bit numbers has a trivial structure and can be encoded in just 6 bytes.

* The list of all ~252K Unicode Code Points (32-bit numbers between 0 and 0x10ffff) is not structural trivial, but contains a lot of contiguous intervals. It can be compressed to a list of 700 integers, i.e. reduced by 99.7% (with GNU gzip one gets only up to ~50% compression when applied to the same data set).

* There are 91 Fibonacci numbers greater than 1 fitting into 64 bit. Lists like this are hard to compress without knowledge of the special construction rule, because the numbers increase enormously fast. Here, _IntZip_ achieves about 30% reduction by way of bit compression, wich is roughly the same reduction as obtained by general compression algorithms.

## Efficiency Considerations

_IntZip_ is designed to provide encoding, decoding and containment testing nearly within a single loop through the given list and without the requirement of dynamic memory allocation, beside from allocating the result list itself. See [Algorithm Description](https://github.com/boethin/intzip/wiki/Algorithm-Description) for details.

## C++ Implementation

The public interface is provided by the header `intzip.h`. The implementation provides functions templates for encoding, decoding and containment testing.

### Encoding

```c++

    template<typename T>
    void intzip::encode(const std::vector<T> &input, std::vector<T> &encoded);
```

Encodes a strictly increasing list of integers and stores the result in the given output vector.

### Decoding

```c++

    template<typename T>
    void intzip::decode(const std::vector<T> &encoded, std::vector<T> &output);
```

Decodes a list that has been encoded before and stores the result in the given output vector.

### Containment Testing

```c++

    template<typename T>
    bool intzip::contains(const std::vector<T> &encoded, const T test);
```

Determine from an encoded list whether or not a test value is contained within the original list without decoding.

### Specializations

Specializations for the function templates are provided for the following types:

* unsigned 16 bit integers of type `T = uint16_t`
* unsigned 32 bit integers of type `T = uint32_t`
* unsigned 64 bit integers of type `T = uint64_t`

The library may be compiled with or without C++11 support. When C++11 is enabled, the integer types are in the `std::` namespace (i.e. from `<cstdint>`), otherwise the C-types from `<stdint.h>` are used.

## Dependencies and portability considerations

The C++ implementation does not depend on any other library. Please [report](https://github.com/boethin/intzip/issues) any portability issues.

## Sample Usage

The following test program demonstrates the usage of the library:

```C++

    // test.cpp
    #include <iostream> // std::cout
    #include <vector> // std::vector
    #include <cstdint> // std::uint32_t
    #include <intzip.h> // intzip::encode
    using namespace std;

    int main() {
        vector<uint32_t> v = {1, 2, 3, 4, 5, 6, 7, 8, 9}, e;
        intzip::encode(v,e);
        for (vector<uint32_t>::iterator it = e.begin(); it != e.end(); ++it)
            cout << hex << *it << endl;
        return 0;
    }
```

With GNU g++, you may compile and run the program as follows:
```

    $ g++ -Wall -std=c++11 -lintzip -o test test.cpp && ./test
    $ 8010100
```

That, is, the list of 32 bit integers 1 .. 9 is encoded into the single integer 0x8010100. This integer may be then passed to the  `intzip::decode` function to get the original list back.


