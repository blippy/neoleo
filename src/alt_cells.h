#pragma once

/* An alternative implementation of the cells collection.
 * Introduced my mcarter 05-Sep-2017, and as yet unfinished
 */


extern bool use_alt_cells;
bool run_alt_cells_tests();
struct cell* alt_find_cell(int row, int col);
