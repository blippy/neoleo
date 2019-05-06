# perl inspired spreadsheet

# notes on grammars:
# tokens ignore whitespace
# rules  do not

grammar F {
	token TOP { (<num>|<op>)* }
	rule num { \d+ }
	rule op { '+' | '-' | '*' }
}

say F.parse("12 + 134");
