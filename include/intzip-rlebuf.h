/**
 * This file is part of intzip.
 *
 * Copyright (C) 2015 Sebastian Boethin <boethin@xn--domain.net>
 *
 * intzip is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * intzip is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with intzip.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef ___INTZIP_RLEBUF_H___
#define ___INTZIP_RLEBUF_H___

#include "intzip-stdint.h"
#include "intzip-def.h"
#include "intzip-uint.h"
#include "intzip-bitstore.h"

// uncomment to disable assert()
// #define NDEBUG
#include <cassert>

using namespace std;

namespace intzip {

// base class
template<typename T, class S>
class rlebuf {
protected:

  static const T magic_esc = 22; // magic 5 bit 10110
  static const uint8_t magic_bits = 5;
  static const uint8_t min_bitsize = 10;

	rlebuf(bitstore<S> &store, size_t length, uint8_t bits)
	  : store(store),
      length(length),
      count(0),
      bits(bits)
	{
    assert(length > 0);
    assert(bits > 0);

    // bitsize is the lowest multiple of bits greater or equal to min_bitsize
    bitmul = min_bitsize / bits + (min_bitsize % bits != 0), bitsize = bits * bitmul;
    uint8_t unmagic_bits = bitsize - magic_bits;
    bit_mask = uint<T>::bitmask(bits),
      rle_unmask = uint<T>::bitmask(unmagic_bits),
      rle_mask = uint<T>::bitmask(magic_bits,unmagic_bits),
      rle_esc = magic_esc << unmagic_bits;
  }
	
	virtual ~rlebuf() {}

protected:
	bitstore<S> &store;
	size_t length, count;
	uint8_t bits;

  uint8_t bitmul, bitsize;
	T bit_mask, rle_unmask, rle_mask, rle_esc;

}; // class rlebuf

template<typename T, class S>
class rlebuf_writer : public rlebuf<T,S> {
public:

  rlebuf_writer(bit_writer<T,S> &store, size_t length, uint8_t bits)
    : rlebuf<T,S>(store,length,bits),
      buf(0),
      pre_buf(0),
      buf_bits(0),
      pre_buf_bits(0),
      have_pre(false),
      rep_count(0)
  {}

  void append(T val)
  {
    this->buf = (this->buf << this->bits) | (val & this->bit_mask);
    this->buf_bits += this->bits;
    
    if (++this->count < this->length && this->buf_bits < this->bitsize)
      return; // expect more

    // input buf completed
    if (this->have_pre) { // pre_buf defined
      if (this->rep_count < this->rle_unmask && this->buf_bits == this->bitsize &&
          this->buf == this->pre_buf) { // equality detected
        ++this->rep_count;
      }
      else {
        this->flush();
      }
    }

    this->pre_buf = this->buf, this->pre_buf_bits = this->buf_bits;
    this->buf = 0, this->buf_bits = 0, this->have_pre = true;
    
    if (this->count == this->length) { // finish
      this->flush();
      if (this->buf_bits > 0) { // empty buf
        this->bit_append(this->buf, this->buf_bits);
      }
    }
  }
  
private:

  void flush()
  {
    if (this->pre_buf_bits)
    {
      if (this->rep_count > 1 || ((this->pre_buf & this->rle_mask) == this->rle_esc)) {
        // we have either 3 or more equal elements or an element that needs to be escaped
        this->bit_append( (this->rle_esc | this->rep_count ), this->bitsize );
      }
      else if ( this->rep_count == 1 ) { // only 2 elements were equal
        this->bit_append(this->pre_buf, this->pre_buf_bits);
      }
      this->bit_append(this->pre_buf, this->pre_buf_bits);
      this->pre_buf_bits = 0;
      this->rep_count = 0;
    }
  }

  void bit_append(const T val, const uint8_t bits)
  {
    ((bit_writer<T,S> &)this->store).append(val,bits);
  }


protected:
	T buf, pre_buf;
	uint8_t buf_bits, pre_buf_bits;
	bool have_pre;
	T rep_count;

};

//
// rlebuf_reader
//
template<typename T, class S>
class rlebuf_reader : public rlebuf<T,const S> {
public:

  rlebuf_reader(bit_reader<T,S> &store, size_t length, uint8_t bits)
    : rlebuf<T,const S>(store,length,bits),
      buf(0),
      pre_buf(0),
      rep_count(0),
      buf_bits(0),
      buf_len(0)
  {}

  T fetch(void)
  {
    ++this->count;

    if (this->buf_bits == 0) // buf needs to be filled
    {
      this->buf_bits = this->bitsize;
      if (this->rep_count > 0) {
        this->buf = this->pre_buf;
        this->rep_count--;
      }
      else {
        if ( this->count + this->bitmul > this->length ) {
          this->buf_bits = this->bits * ( this->length - this->count + 1 );
        }
        this->buf = this->bit_fetch(this->buf_bits);
        
        if ( (  this->buf_bits == this->bitsize) && (this->buf & this->rle_mask) == this->rle_esc)
        {
          this->rep_count = this->buf & this->rle_unmask; // unescape
          this->buf = this->pre_buf = this->bit_fetch(this->bitsize);
        }
      }
      this->buf_len = this->buf_bits;
    }
    
    assert(this->buf_len > 0);
    assert(this->buf_bits > 0);
    
    this->buf_bits -= this->bits;
    return (this->buf >> this->buf_bits) & this->bit_mask;
  }

private:

  T bit_fetch(uint8_t bits)
  {
    return ((bit_reader<T,S> &)this->store).fetch(bits);
  }

protected:
  T buf, pre_buf, rep_count;
  uint8_t buf_bits, buf_len;

};


} // namespace intzip

#endif

