#!/usr/bin/perl
use strict;
use warnings;

# generate intzip autotest cases

use constant TESTS_DIR => 'tests';
use constant TESTDATA_DIR => 'testdata';

my %create = (

  encode_decode => sub {
    my ($at_fh,$data_fh,$filename,$setup,$list) = @_;

    # add to .at test category file
    printf $at_fh "# Check whether encoding/decoding of $setup maps to itself.\n";
    printf $at_fh "AT_CHECK_ENCODE_DECODE([%s],[%s])\n\n", $setup, $filename;

    # create testdata
    printf $data_fh "%x\n", $_ foreach @$list;
  },

);

sub at_category($$@) {
  my ($category,$banner) = (shift,shift);

  # init .at test category file
  my $at_path = +TESTS_DIR."/$category.at";
  open my $at_fh, '>', $at_path or die $!;
  print $at_fh "# Tests for the $category category.\n";
  printf $at_fh "#\n# - This file was auto-generated by %s -\n#\n", $0;
  printf $at_fh "AT_BANNER([[%s.]])\n\n", $banner;

  foreach my $t ( @_ ) {
    my ($type,$name,$setup) = map { $t->{$_} } qw(type name setup);
    
    # name may be either a custom name or is generated from the list
    unless ( defined $name) {
      $name = sprintf (join ',', map { $_ > 9 ? (sprintf '0x%X', $_) : $_ } @{$t->{data}});
      $setup = sprintf '[[%s]]', $name unless defined $setup;
    }

    # create testdata
    my $filename = $name;
    $filename =~ s/[^a-z0-9]/_/gi;
    $filename = $category."_$filename";
    my $data_path = +TESTDATA_DIR."/$filename";
    open my $data_fh, '>', $data_path or die $!;
    &{$create{$type}}($at_fh,$data_fh,$filename,$setup,$t->{data});
    close $data_fh;
  }

  close $at_fh;
}

# tests

at_category empty => 'Empty List Tests',
  { type => 'encode_decode', name => 'empty', setup => 'Empty', data => [] };

at_category singleton => 'Singleton List Tests',
  map { { type => 'encode_decode', data => [$_] } } (
    (0 .. 8), 23, 0xff, 0x100, 0xffff, 0x10000, 0xf2a759e, 0xfffffffe, 0xffffffff
  );

at_category short => 'Short List Tests',
  map { { type => 'encode_decode', data => $_ } } (
    ( map { [0,$_] } (1,2,0x100,0xffff,0xffffffff) ),
    ( map { [1,$_] } (2,3,23,0xffff,0xffffffff) ),
    ( map { [$_,0xffffffff] } (0,1,0xffff,0xfffffffe) ),
    ( map { [1,2,$_] } (3,4,23) ),
    [ ( 0 .. 5 ) ],
    [ ( 0xff .. 0x102 ) ],
    [ ( 0xfffe .. 0x10001 ) ],
  );

__END__
