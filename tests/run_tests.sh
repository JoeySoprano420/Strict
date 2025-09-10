#!/bin/bash
set -e

# Location of compiler
COMPILER=../build/strictc

# Directory setup
EXAMPLES=../examples
EXPECTED=./expected_outputs
OUTDIR=./out

mkdir -p $OUTDIR

echo "=== Running Strict Tests ==="

# Test each .strict file
for src in $EXAMPLES/*.strict; do
    base=$(basename $src .strict)
    exe=$OUTDIR/$base.exe
    out=$OUTDIR/$base.out
    expect=$EXPECTED/$base.txt

    echo ">> Testing $base"

    # Compile
    $COMPILER $src -o $exe

    # Run program (with no input, or redirect as needed)
    if [[ -f $EXPECTED/$base.in ]]; then
        $exe < $EXPECTED/$base.in > $out
    else
        $exe > $out
    fi

    # Diff outputs
    if diff -u $expect $out; then
        echo "   PASS"
    else
        echo "   FAIL"
        exit 1
    fi
done

echo "=== All tests passed ==="
