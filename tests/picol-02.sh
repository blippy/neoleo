#!/bin/sh

# 2025-10-03 test of picol source'ing
$BUILDDIR/src/neoleo -H > out/picol-02.txt <picol-02.tcl

diff out/picol-02.txt $SRCDIR/verified/picol-02.txt
