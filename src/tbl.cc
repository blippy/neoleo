#include <iostream>

#include "cell.h"
#include "io-utils.h"
#include "lists.h"

using std::cout;

typedef struct RC_t { int r; int c;} RC_t;

/*
 * highest_row() and highest_col() should be expected to 
 * overallocate the number of rows and columns in the 
 * spreadsheet. The true number of rows and columns must 
 * be calculated
 *
 * TODO this is likely to be a very useful function
 *
 */

RC_t
ws_extent()
{
	int capacity_r = highest_row(), capacity_c = highest_col();
	//cell* m[capacity_r][capacity_c] = {};
	int size_r = 0, size_c = 0;
	for(int r=0; r<capacity_r; ++r)
		for(int c=0; c < capacity_c; ++c){
			cell * cp = find_cell(r+1, c+1);
			if(vacuous(cp)) continue;
			size_r = std::max(size_r, r+1);
			size_c = std::max(size_c, c+1);
			//m[r][c] = cp;
		}
	return RC_t{size_r, size_c};
}


char jst_char(cell* cp)
{
	if(vacuous(cp)) return 'r';

	switch(cp->get_cell_jst()) {
		case JST_LFT:
			return 'l';
		case JST_CNT:
			return 'c';
		default:
			return 'r';
	}
}

void 
tbl()
{
	cout << ".TS\n";

	RC_t end{ws_extent()};

	for(int r =0; r< end.r; ++r) {

		if(r>0) cout << ".T&\n";

		// print justifications
		for(int c=0; c<end.c; ++c) {
			cell* cp = find_cell(r+1, c+1);
			char jst = jst_char(cp);
			cout << jst;
		}
		cout << ".\n";

		// print values
		for(int c=0; c<end.c; ++c) {
			cell* cp = find_cell(r+1, c+1);
			if(!vacuous(cp)) cout << print_cell(cp);
				
			//cout << print_cell(cp);
			if(c+1<end.c)	cout << "\t";
		}
		cout << "\n";
	}

	cout << ".TE\n";

}
