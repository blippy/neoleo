#!/bin/sh

# 2025-10-03 simple test of picol
$BUILDDIR/src/neoleo -H > out/picol-01.txt <picol-01.tcl

diff out/picol-01.txt $SRCDIR/verified/picol-01.txt
