#!/bin/bash
set -e
set -f
set -o pipefail
LC_NUMERIC=

# Comparison script for IntZip encoding.
# See https://github.com/boethin/intzip/wiki for more information.
#
# testfiles have the following naming convention:
# - *.iz             data is IntZip encoded
# - *.hex            data containing hex numbers, line by line
# - *.bin            data containing big-endian encoded numbers (bitsize required)
# - *.u16|u32|u64    integer bitsize
#
# To check all testdata files run:
# ` find testdata -type f | sort | while read f ; do bash compare.sh $f ; done`
#
testfile=$1
test -f "$testfile" || { echo 1>&2 "usage: $0 <testfile>"; exit 1; }

intzip=./src/intzip
intzip_args=

# strip .iz extension
if [ "${testfile##*.}" = "iz" ]
then
  encoded=1
  filename="${testfile%.*}"
else
  filename=$testfile
fi

# strip hex/bin extension
if [ "${filename##*.}" = "bin" ]
then
  binary=1
  intzip_args=$intzip_args" --binary"
elif [ "${filename##*.}" = "hex" ]
then
  binary=
else
  echo 1>&2 "filename must contain either .hex or .bin extension"
  exit 1
fi
filename="${filename%.*}"

# check for bitsize extension
if [ -n "$encoded" -o -n "$binary" ]
then
  if [ "${filename##*.}" = "u16" ]
  then
    bitsize=2
    intzip_args=$intzip_args" --u16"
  elif [ "${filename##*.}" = "u32" ]
  then
    bitsize=4
    intzip_args=$intzip_args" --u32"
  elif [ "${filename##*.}" = "u64" ]
  then
    bitsize=8
    intzip_args=$intzip_args" --u64"
  else
    echo 1>&2 "filename must contain a bitsize extension"
    exit 1
  fi
fi

if [ -n "$encoded" ]
then
  # encoded file was given
  if [ -n "$binary" ]
  then
    # count binary elements
    encsize=$(expr $(cat "$testfile" | wc -c) / $bitsize)
    decsize=$(expr $($intzip -d $intzip_args $testfile | wc -c) / $bitsize)
  else
    # count lines
    encsize=$(cat "$testfile" | wc -l)
    decsize=$($intzip -d $intzip_args $testfile | wc -l)
  fi
else
  # decoded file was given
  if [ -n "$binary" ]
  then
    # count binary elements
    decsize=$(expr $(cat "$testfile" | wc -c) / $bitsize)
    encsize=$(expr $($intzip $intzip_args $testfile | wc -c) / $bitsize)
  else
    # count lines
    decsize=$(cat "$testfile" | wc -l)
    encsize=$($intzip $intzip_args $testfile | wc -l)
  fi
fi

# output
if [ "$encsize" -lt "$decsize" ]
then
  rate=$(printf "%.2f%%" $(echo "(1 - $encsize / $decsize) * 100" | bc -l))
else
  rate="no compression"
fi
printf "%s: %d items encoded to %d items: compression: %s\n" $(basename "$testfile") "$decsize" "$encsize" "$rate"


