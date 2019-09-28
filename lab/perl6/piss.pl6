# perl inspired spreadsheet

# notes on grammars:
# tokens ignore whitespace
# rules  do not

grammar F {
	token TOP { (<num>|<op><funcall>)* }
	#rule num { \d+ }
	#rule num { [0..9]* \.?[0-9]+([eE][-+]?[0-9]+)?$. }
	rule num { \d* '.'? \d+  }
	rule op { '+' | '-' | '*' }
	token funcall { \w+ '(' <num> ')' }
}

say F.parse("12.0+sign(  134)");
