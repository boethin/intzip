#!/bin/bash
#
# Downloads UnicodeData.txt and output each Code Point's decimal value.
# Pipe through `perl -ne 'print pack "N",$_'` in order to get u32 binary data.
#
ucd_gz=UnicodeData.txt.gz
ucd_url=http://www.unicode.org/Public/UCD/latest/ucd/UnicodeData.txt

# ensure cache
test -f "$ucd_gz" || curl "$ucd_url" | gzip -c9 - >$ucd_gz

# Some consecutive lines refer to intervals, e.g.:
#   3400;<CJK Ideograph Extension A, First>;Lo;0;L;;;;;N;;;;;
#   4DB5;<CJK Ideograph Extension A, Last>;Lo;0;L;;;;;N;;;;;
first=
cat "$ucd_gz" | gunzip -c - | cut -f1,2 -d';' | while read s
do
  n=$((16#${s%%;*}))
  case $s in
    *"First>")
      first=$n
      ;;
    *"Last>")
      for k in $(seq $first $n);
      do
        echo $k
      done
      ;;
    *)
      echo $n
      ;;
  esac
done

