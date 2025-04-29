#!/usr/bin/env python
# Hmmm. doesn't seem to work

from io import StringIO
from sylk_parser import SylkParser

parser = SylkParser("bold.slk")

fbuf = StringIO()
parser.to_csv(fbuf)

test_results = fbuf.getvalue()
print(test_results)
