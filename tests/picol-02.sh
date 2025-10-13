#!/bin/sh

# 2025-10-03 test of picol source'ing
$BUILDDIR/src/neoleo -H > out/picol-02.rep <picol-02.tcl

diff out/picol-02.rep $SRCDIR/verified/picol-02.rep
