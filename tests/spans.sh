#!/usr/bin/env bash
# 2025-04-19 created
# simple test of the spans functionality
# Can it simply put out what it sucked in?

. ./neotests-env.sh

IN=verified/spans.oleo
OUT=out/spans.oleo
echo t | $NEO -H $IN >$OUT
diff $IN $OUT
exit $?

