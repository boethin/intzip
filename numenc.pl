use strict;
use warnings;
no warnings 'portable';  # Support for 64-bit ints required



# zzzzzzzz wwwwwwww vvvvvvvv uuuuuuuu (32 bit network order)
# 
# Encoded:
# Auuuuuuu Bvvvvvv vvCwwwww wwwDzzzz zzzz (total max. 36 bit
# 
# If 0 <= n < 0x80: A=0 (7 bit number, 8 bits total)
# Y76543210
# 
# If 0x80 <= n < 0x4000: A=1, B=0 (14 bit number, 16 bits total)
# 1xxxxxxx Nxxxxxx8
# 
# If 0x4000 <= n < 0x200000: A=1, B=1, C=0 (21 bit number, 24 bits total)
# 1xxxxxxx 1xxxxxxx 0xxxxxxx
# 
# If 0x200000 <= n < 0x10000000: A=1, B=2, C=1, D=0 (28 bit number, 32 bits total)
# 1xxxxxxx 1xxxxxxx 1xxxxxxx 0xxxxxxx
# 
# If 0x10000000 <= n < 0x100000000: A=1, B=2, C=1, D=1 (32 bit number, 36 bits total)
# 1xxxxxxx 1xxxxxxx 1xxxxxxx 1xxxxxxx 1xxxxxxx xxxx
# 

# 0x100000000
# 0x800000000

sub bitsneed {
	my ($n) = @_;

# 	return 8 if $n < 0x80;
# 	return 16 if $n < 0x4000;
# 	return 24 if $n < 0x200000;
# 	return 32 if $n < 0x10000000;
# 	return 36;
	
	my $bitsize = 64;
	
	my $b = 0x80;
	my $l = int($bitsize/7);
	for (my $i = 1; $i <= $l; $i++) {
    if ($n < $b) {
      return $i*8;
    }
    $b <<= 7;
  }
  return $bitsize + $l;
	
}

sub ceillog2 {
  my ($n) = @_;
  my $lg = 0;
  while ($n) {
    $n >>= 1;
    $lg++;
  }
  $lg;
}

# n: 00000082 [00000000] [00000000] [00000000] [10000010]
# append: [10000010] = ($n & 0x7F) | 0x80
# append: [00000001] = ($n >> 7) | (($n & 0x80) << 1)

# 4294967295

#  for n in {1..100..10000}; do perl numenc.pl $n ; done


  #// number encoding:
  #//
  #// An integer is splitted into 7-bit blocks where each block is extended to 8 bit
  #// by an additional forward-bit that determines whether or not there are more
  #// blocks left. Thus, a number n with 0 <= n < 2^7 takes 8 bit, while in case of
  #// 2^7 <= n < 2^14 it takes 16 bit and so forth. 32-bit values above 2^28
  #// take 36 bit because of 4 forward-bits.

sub enc32 {
	my ($n,$enc,$i,$off) = @_;
	
	my @enc;
	
	my $bitsize = 16;
  my $u = $bitsize - 7;
  my $lastbits = $bitsize % 7;

  # 16 = 2*7 + 2 (16 == 2 (mod 7)) => $lastbits16 = 2
  # 32 = 4*7 + 4 (32 == 4 (mod 7)) => $lastbits32 = 4
  # 64 = 9*7 + 1 (64 == 1 (mod 7)) => $lastbits64 = 1
  printf "# bitsize=%d: u=%d, lastbits=%d\n",$bitsize,$u,$lastbits;

  do {
    my $c = 0;
    my $b = 1;
    my $s = 0;
    while (1) {

      if ( $s > $u ) {
        $c += $lastbits;
        last;
      }

      $b <<= 7;
      if ( $n < $b ) {
        $c += 8;
        last;
      }

      $c += 8;
      $s += 7;
    }
    printf "# bitlen: %d\n",$c;

  };


  my $k = 0;
	do {
    my $b = 1;
    my $s = 0;
    while ( 1 ) {
      #printf "# s=%d, b=%X\n",$s,$b;
      my $t = ($n >> $s) & 0x7F;
      if ( $s > $u ) {
        printf "# append($lastbits): %02X [%08b]\n",$t,$t;
        $k += $lastbits;
        push @enc, $t;
        last;
      }

      $b <<= 7;
      if ( $n < $b ) {
        printf "# append(8): %02X [%08b]\n",$t,$t;
        $k += 8;
        push @enc, $t;
        last;
      }
      
      $t |= 0x80;
      printf "# append(8): %02X [%08b]\n",$t,$t;
      push @enc, $t;
      $k += 8;
      $s += 7;
      
    }
  };
  
  print "--\n";

  my $m = 0;
  do {
    my $s = 0;
    while (@enc) {
      my $t = shift @enc;
      my $bits = $s <= $u ? 8 : $lastbits;
      printf "# fetch($bits): %02X [%08b]\n",$t,$t;
      $m |= ($t & 0x7F) << $s;
      last unless $t & 0x80;
      $s += 7;
    }
#    printf "# final: %08X\n",$m;
  };
  die sprintf "failed: %X",$n unless $m == $n;


  
#   my $t;
#
# 	$t = $n & 0x7F;
# 	if ($n < 0x80) {
# 		printf "# append(1): %02X [%08b]\n",$t,$t;
# 		return;
# 	}
#   $t |= 0x80;
# 	printf "# append(1): %02X [%08b]\n",$t,$t;
#
#   $t = ($n >> 7) & 0x7F;
# 	if ($n < 0x4000) {
# 		printf "# append(2): %02X [%08b]\n",$t,$t;
# 		return;
# 	}
#   $t |= 0x80;
# 	printf "# append(2): %02X [%08b]\n",$t,$t;
#
#   $t = ($n >> 14) & 0x7F;
# 	if ($n < 0x200000) {
# 		printf "# append(3): %02X [%08b]\n",$t,$t;
# 		return;
# 	}
#   $t |= 0x80;
# 	printf "# append: %02X [%08b]\n",$t,$t;
#
#   $t = ($n << 21) & 0xFF;
# 	if ($n < 0x10000000) {
# 		printf "# append: %02X [%08b]\n",$t,$t;
# 		return;
# 	}
#   $t = 0x80 | $n;
# 	printf "# append: %02X [%08b]\n",$t,$t;


}

my $n = eval(shift @ARGV);
#for (my $n = 0; $n <= 0xffffffff; $n++) {

printf "# n: %08X [%08b] [%08b] [%08b] [%08b]\n", $n,
  (($n & 0xFF000000) >> 24), (($n & 0xFF0000) >> 16), (($n & 0xFF00) >> 8), (($n & 0xFF));

my $l = bitsneed($n);
printf "# len: %d\n",$l;

enc32($n);

#}






