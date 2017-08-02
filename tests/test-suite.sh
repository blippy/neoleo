#!/usr/bin/env bash

# just do these next two tests to confirm some kind of correct setup
echo "PASS null test 1" 
echo "PASS null test 2"


neoleo -T

rm -f out/*
neoleo ../examples/ref.oleo -H <<< "save-spreadsheet out/foo.oleo"
