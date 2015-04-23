#ifndef ___INTZIP_RLEBUF_H___
#define ___INTZIP_RLEBUF_H___

#include "intzip-stdint.h"
#include "intzip-def.h"
#include "intzip-uint.h"
#include "intzip-bitstore.h"

//#define TxFORMAT "016" PRIx64
//#define TxFORMAT "08" PRIx32

//#define DEBUGG(...) fprintf(stderr, __VA_ARGS__ )
//#define DEBUGG(...)

// uncomment to disable assert()
// #define NDEBUG
#include <cassert>

#include <stdio.h>

using namespace std;

namespace intzip {

// base class
template<typename T, class S>
class rlebuf {
protected:

  static const uint8_t min_bitsize = 10;
  static const uint8_t magic_esc = 22; // magic 5 bit 10110
  static const uint8_t magic_bits = 5;

	rlebuf(bitstore<S> &store, size_t length, uint8_t bits)
	  : store(store),
      length(length),
      count(0),
      bits(bits)
	{
    assert(length > 0);
    assert(bits > 0);
	
    // bitsize is the lowest multiple of bits greater or equal to min_bitsize
    if (bits >= min_bitsize) {
      bitsize = bits, bitmul = 1;
    }
    else {
      bitmul = min_bitsize / bits;
      if (bits * bitmul < min_bitsize)
        bitmul++;
      bitsize = bits * bitmul;
    }

    //DEBUGG("$ [rlebuf] bits = %d: bitsize = %d\n", bits, bitsize);
    bit_mask = uint<T>::bitmask(bits);
	  rle_esc = (T)magic_esc << (bitsize - magic_bits);
	  //DEBUGG("$ [rlebuf] rle_esc =  0x%" TxFORMAT "\n",rle_esc);
    //rle_mask = ((1 << magic_bits) - 1) << (bitsize - magic_bits);
    rle_mask = uint<T>::bitmask(magic_bits,bitsize - magic_bits);
    //DEBUGG("$ [rlebuf] rle_mask = 0x%" TxFORMAT "\n",rle_mask);
    //rle_unmask = ~rle_mask;
    //rle_max =
    rle_unmask = uint<T>::bitmask(bitsize - magic_bits);
    
//     rle_max = (1 << (bitsize - magic_bits)) - 1;
//     if (rle_max != rle_unmask) {
//       fprintf(stderr,"\nrle_max = %016x != %016x = rle_unmask\n\n",rle_max,rle_unmask);
//       assert(false);
//     }
    //assert(rle_unmask == rle_max);
    //DEBUGG("$ [rlebuf] rle_max = %d\n",rle_max);
  }
	
	virtual ~rlebuf() {}

protected:
	bitstore<S> &store;
	size_t length;
  size_t count;
	uint8_t bits;

  uint8_t bitmul;
	uint8_t bitsize;
	T bit_mask;
	T rle_mask;
	T rle_unmask;
	T rle_esc;
	//T rle_max;

}; // class rlebuf

template<typename T, class S>
class rlebuf_writer : public rlebuf<T,S> {
public:

  rlebuf_writer(bit_writer<T,S> &store, size_t length, uint8_t bits)
    : rlebuf<T,S>(store,length,bits),
      buffer(0),
      pre_val(0),
      have_pre_val(false),
      buffer_bits(0),
      pre_val_bits(0),
      rep_c(0)
  {}

  void append(T val)
  {
    //assert((this->bit_mask & val) == val);
    
    //DEBUGG("$ [append][%lu] 0x%" TxFORMAT "\n",this->count,val);
    
//     this->buffer |= (val << (this->buffer_bits));
//     this->buffer_bits += this->bits;
    
    this->buffer = (this->buffer << this->bits) | (val & this->bit_mask);
    this->buffer_bits += this->bits;
    
    //DEBUGG("$ [append] buffer = 0x%" TxFORMAT " (buffer_bits = %d)\n",this->buffer,this->buffer_bits);
    if (++this->count < this->length && this->buffer_bits < this->bitsize)
      return; // expect more

    // input buffer completed
    if (this->have_pre_val) { // pre_val defined
      if ( (this->rep_c < this->rle_unmask) && (this->buffer_bits == this->bitsize) && (this->buffer == this->pre_val) ) {
        // equality detected
        this->rep_c++;
        //DEBUGG("$ [append] rep_c increment: %d\n",this->rep_c);
      }
      else {
        this->flush();
      }
    }

    this->pre_val = this->buffer, this->pre_val_bits = this->buffer_bits;
    this->buffer = 0, this->buffer_bits = 0;
    this->have_pre_val = true;
    
    if (this->count == this->length) // finish
    {
      //DEBUGG("$ finish\n");
      this->flush();
      if (this->buffer_bits > 0) { // empty buffer
        this->bit_append(this->buffer, this->buffer_bits);
      }
    }
  }
  
private:

  void flush()
  {
    if (this->pre_val_bits)
    {

      if ( this->rep_c > 1 || ( (this->pre_val & this->rle_mask) == this->rle_esc ) ) {
        // we have either 3 or more equal elements or an element that needs to be escaped
        //DEBUGG("$ [flush] escape: rep_c = %d (pre_val_bits = %d, bitsize=%d)\n",rep_c,this->pre_val_bits,this->bitsize);
        this->bit_append( (this->rle_esc | this->rep_c ), this->bitsize );
      }
      else if ( this->rep_c == 1 ) {
        // only 2 elements were equal
        //DEBUGG("$ [flush] not escape: rep_c = %d\n",rep_c);
        this->bit_append(this->pre_val, this->pre_val_bits);
      }
      this->bit_append(this->pre_val, this->pre_val_bits);
      this->pre_val_bits = 0;
      this->rep_c = 0;
    }
  }

  void bit_append(const T val, const uint8_t bits)
  {
    //DEBUGG("$ [bit_append] 0x%" TxFORMAT " (%d bits)\n",val,bits);
    ((bit_writer<T,S> &)this->store).append(val,bits);
  }


protected:
	T buffer;
	T pre_val;
	bool have_pre_val;
	uint8_t buffer_bits;
	uint8_t pre_val_bits;
	T rep_c;

};

//
// rlebuf_reader
//
template<typename T, class S>
class rlebuf_reader : public rlebuf<T,const S> {
public:

  rlebuf_reader(bit_reader<T,S> &store, size_t length, uint8_t bits)
    : rlebuf<T,const S>(store,length,bits),
      buffer(0),
      pre_val(0),
      buffer_bits(0),
      buffer_len(0),
      rep_c(0)
  {}

  T fetch(void)
  {
    T val = 0;
    this->count++;

    if (this->buffer_bits == 0) // buffer needs to be filled
    {
      this->buffer_bits = this->bitsize;
      if (this->rep_c > 0) {
        //DEBUGG("$ [fetch] buffer: 0x%08x (rep_c = %d)\n",this->pre_val,this->rep_c);
        this->buffer = this->pre_val;
        this->rep_c--;
      }
      else {
        if ( this->count + this->bitmul > this->length ) {
          this->buffer_bits = this->bits * ( this->length - this->count + 1 );
        }
        this->buffer = this->bit_fetch(this->buffer_bits);
        
        if ( (  this->buffer_bits == this->bitsize) && (this->buffer & this->rle_mask) == this->rle_esc)
        {
          this->rep_c = this->buffer & this->rle_unmask; // unescape
          this->buffer = this->pre_val = this->bit_fetch(this->bitsize);
          //DEBUGG("$ [fetch] buffer: 0x%08x (rep_c start = %d)\n",this->pre_val,this->rep_c);
        }
        else {
          //DEBUGG("$ [fetch] buffer: 0x%08x\n",this->pre_val);
        }
      }
      this->buffer_len = this->buffer_bits;
    }
    
    assert(this->buffer_len > 0);
    assert(this->buffer_bits > 0);
    
//     val = (this->buffer >> (this->buffer_len - this->buffer_bits)) & this->bit_mask;
//     this->buffer_bits -= this->bits;

    this->buffer_bits -= this->bits;
    val = (this->buffer >> this->buffer_bits) & this->bit_mask;


    return val;
  }

private:

  T bit_fetch(uint8_t bits)
  {
    T val = ((bit_reader<T,S> &)this->store).fetch(bits);
    //DEBUGG("$ [bit_fetch] 0x%" TxFORMAT " (%d bits)\n",val,bits);
    return val;
    //return ((bit_reader<T,S> &)this->store).fetch(this->bitsize);
  }

protected:
  T buffer;
	T pre_val;
  uint8_t buffer_bits;
  uint8_t buffer_len;
	T rep_c;



};


} // namespace intzip

#endif

