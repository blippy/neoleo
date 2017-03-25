\ an example of words you may want upfront
: down-cell "down-cell" 4xcmd ;
: up-cell "up-cell" 4xcmd ;

\ let's create words for saving the spreadsheet
256 string sname \ spreadsheet name
: sssn ( str -- ) sname strcpy ; \ set spreadsheet name
"unnamed.oleo" sssn \ set a default spreadsheet name
500 string cmd \ a buffer for holding commands
: mk-sss-cmd  \ make spreadsheet command
	"save-spreadsheet " cmd strcpy 
	sname cmd  strcat ;
: sss cmd 4xcmd ; \ save spreadsheet
