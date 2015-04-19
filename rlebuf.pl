use strict;
use warnings;
use Data::Dumper;

package rlebuf;

use constant min_bitsize => 10;
use constant magic_esc => 0x16; # magic 5 bit 10110
use constant magic_bits => 5;

sub new {
  my $class = shift;
  my ($bits) = @_;
  bless init(@_) => $class;
}

sub append {
  my $this = shift;
  my ($val) = @_;
  printf "# [append] val: %016b\n", $val;
  
  $this->{buffer} |= ( ($val & $this->{bitmask}) << $this->{buffer_bits} );
  $this->{buffer_bits} += $this->{bits};
  printf "# [append] buffer: %016b (buffer_bits = %d)\n", $this->{buffer}, $this->{buffer_bits};
  
  if ( $this->{buffer_bits} == $this->{bitsize} ) {
    $this->flush;
  }
  
  print "--\n";
}

sub flush {
  my $this = shift;
  
  printf "# [flush] buffer_bits = %d\n", $this->{buffer_bits};
  if ($this->{buffer_bits} == 0) {
    return 0;
  }
  
  #printf "# [flush] buffer: %016b (buffer_bits = %d)\n", $this->{buffer}, $this->{buffer_bits};
  

}

sub init {
  my ($bits) = @_;
  my $bitmask = (1 << $bits) - 1;
  my $bs = calculate_bitsize($bits);
  my $rep_max = (1 << ( $bs - +magic_bits ));
  my $rle_esc = (+magic_esc << ($bs - +magic_bits));
  my $rle_mask = ((1 << +magic_bits) - 1) << ($bs - +magic_bits);
  my $rle_unmask = ~$rle_mask;
  my $this = {

    bits => $bits,
    bitmask => $bitmask,

    bitsize => $bs,
    rep_max => $rep_max,
    rle_esc => $rle_esc,
    rle_mask => $rle_mask,
    rle_unmask => $rle_unmask,

    buffer => 0,
    buffer_bits => 0,
  };
  printf "# [init] bits = %d, => bitsize = %d\n", $this->{bits}, $this->{bitsize};
  return $this;
}

sub calculate_bitsize {
  # the lowest multiple of bits greater or equal to min_bitsize
  my ($bits) = @_;
  return $bits if $bits >= +min_bitsize;
  my $m = int( +min_bitsize / $bits );
  my $bitsize = $bits * $m;
  $bitsize += $bits if $bitsize < +min_bitsize;
  $bitsize;
}

package main;


my $writer = rlebuf->new(7);
print Dumper($writer);

$writer->append(1);
$writer->append(5);

# min_bitsize = 10
# bs >= 10
#   5 bit esc: 10110 (22 = 0x16)
# + (bs - 5) bit rle_c >= 5
# => rle_esc = (esc << (bs - 5)
# => rep_max = (1 << (bs - 5))

# my $esc = 0x16;   # magic 10110
# my $esc_bits = 5; # magic bits
#
# my $bs = 11;
# printf "bs: %d\n", $bs;
#
# my $bitmask = (1 << $bs) - 1;
# printf "bitmask:     %0".$bs."b\n", $bitmask;
#
# my $rep_max = (1 << ($bs - 5));
# printf "rep_max: %d\n", $rep_max;
#
# my $rle_esc = ($esc << ($bs - $esc_bits));
# printf "rle_esc:     %0".$bs."b\n", $rle_esc;
#
# my $rle_mask = ((1 << $esc_bits) - 1) << ($bs - $esc_bits);
# printf "rle_mask:    %0".$bs."b\n", $rle_mask;
#
# my $rle_unmask = ~$rle_mask;
# printf "rle_unmask:  %0".$bs."b\n", ($rle_unmask & $bitmask);



































