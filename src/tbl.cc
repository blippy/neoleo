#include <iostream>
#include <string>

#include "cell.h"
#include "io-utils.h"
#include "sheet.h"
#include "tbl.h"

using std::cout;
using std::string;



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

void save_csv (const std::string& filename, char sep)
{
	//std::string filename = FileGetCurrentFileName() + ".csv";
	FILE *fp = fopen(filename.c_str(), "w");
	save_dsv(fp, sep);
	fclose(fp);
}

void save_dsv (FILE *fpout, char sep)
{

	auto out = [&](const std::string& s) { fprintf(fpout, "%s", s.c_str()); };

	RC_t end{ws_extent()};
	for(int r =0; r< end.r; ++r) {
		for(int c=0; c<end.c; ++c) {
			cell* cp = find_cell(r+1, c+1);
			if(vacuous(cp))
			       out("");
			else
				out(print_cell(cp));
				
			if(c+1<end.c) out(string{sep});
		}
		out("\n");
	}
}
