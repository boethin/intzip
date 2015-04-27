# IntZip
Fast compression of integer sets.

## Description

_IntZip_ is a lossless compression algorithm for strictly increasing lists of unsigned integers.

A suitable list of integers can be encoded to a (whenever possible) shorter list of the same integer format. An arbitrary set of integers (i.e., a collection of unique numbers where the order does not matter) may be transformed into suitable input data by way of translation and sorting.

The main focus of the algorithm is to encode structural characteristics of a given list, such as coherent intervals and repeated distances. Therefore, the quality of compression depends on diversity rather than size. In most cases, _IntZip_ is able to achieve much better compression results than general compression algorithms on such data sets. For example:

* The complete list of all 65536 16-bit numbers has a trivial structure and can be encoded in just 6 bytes.

* The list of all ~252K Unicode Code Points (32-bit numbers between 0 and 0x10ffff) is not structural trivial, but contains a lot of contiguous intervals. It can be compressed to a list of 700 integers, i.e. reduced by 99.7% (with GNU gzip one gets only up to ~50% compression when applied to the same data set).

* There are 91 Fibonacci numbers greater than 1 fitting into 64 bit. Lists like this are hard to compress without knowledge of the special construction rule, because the numbers increase enormously fast. Here, _IntZip_ achieves about 30% reduction by way of bit compression, wich is roughly the same reduction as obtained by general compression algorithms.

## Efficiency considerations

_IntZip_ is designed to provide encoding, decoding and random access nearly within a single loop through the given list and without the requirement of dynamic memory allocation, beside from the result list itself.

See [Algorithm Description](https://github.com/boethin/intzip/wiki/Algorithm-Description) for details.

## C++ Implementation

Basically, there are 3 function templates provided:

* `template<typename T> void intzip::encode(const std::vector<T> &input, std::vector<T> &encoded);`
  * `input`: A list of strictly increasing integers.
  * `encoded`: An empty vector object wherein the encoded output will be stored.
* `template<typename T> void intzip::decode(const std::vector<T> &encoded, std::vector<T> &output);`
  * `encoded`: A vector containing a previously encoded list of integers.
  * `output`: A vector object wherein the original list will be stored.
* `template<typename T> bool intzip::contains(const std::vector<T> &encoded, const T test);`
  * `encoded`: A vector containing a previously encoded list of integers.
  * `test` A test value.
  * The return value determines whether or not the test value is contained within the original list.

The `contains`function provides an efficient way to determine whether or not a given integer is contained in a list without the need of decoding the whole list.

Specializations for the function templates are provided for the following types:

* unsigned 16 bit integers of type `T = uint16_t`
* unsigned 32 bit integers of type `T = uint32_t`
* unsigned 64 bit integers of type `T = uint64_t`

The library may be compiled with or without C++11 support. When C++11 is enabled, the integer types are in the `std::` namespace (i.e. from `<cstdint>`), otherwise the types from `<stdint.h>` are used.

## Dependencies and portability considerations

The C++ implementation does not depend on any other library. Please [report](https://github.com/boethin/intzip/issues) any portability issues.


