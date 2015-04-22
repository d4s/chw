#!/bin/bash

HUFFMAN="$1"

# in megabytes
FILESIZE=256

PREFIX=test.file

# From urandom -- all symbols in file
RANDFILE=$PREFIX.rnd
# From zero -- only one symbol
ZEROFILE=$PREFIX.zero
# Generated -- completely unbalanced tree
# compile with "cc -std=gnu99 -Wall -pedantic gen_unbalanced_data.c -o gen_unbalanced_data"
UNBFILE=$PREFIX.unb
# Just for fun -- empty file ;-)
EMPTFILE=$PREFIX.empty

if [ -z "$HUFFMAN" ] ; then
    echo "Usage: $0 <huffman_binary>"
    exit 1
fi

set -e

echo Prepare test files:
echo "- from /dev/zero"
[ -f "$ZEROFILE" ] || dd if=/dev/zero of="$ZEROFILE" bs=1M count=$FILESIZE
echo "- from /dev/urandom"
[ -f "$RANDFILE" ] || dd if=/dev/urandom of="$RANDFILE" bs=1M count=$FILESIZE
echo "- generate unbalanced"
[ -f "$UNBFILE" ] || ./gen_unbalanced_data 28 > "$UNBFILE"
echo "- empty"
[ -f "$EMPTFILE" ] || touch "$EMPTFILE"


echo Huffman test started.

test() {

    local FILE="$1"

    echo -n "$FILE compressed in "
    time -f "%U seconds (user time only)" "$HUFFMAN" -c "$FILE" "$FILE".compressed
    echo -n "$FILE decompressed in "
    time -f "%U seconds (user time only)" "$HUFFMAN" -x "$FILE".compressed "$FILE".decompressed
    cmp "$FILE" "$FILE".decompressed || echo "Decompressed file differs from original one!!!"

    rm -f "$FILE".compressed "$FILE".decompressed
}


for INFILE in "$ZEROFILE" "$RANDFILE" "$UNBFILE" "$EMPTFILE" ; do
    test "$INFILE"
done
