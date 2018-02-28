unit cells;


{$mode objfpc}{$H+}

interface



uses
  Classes, SysUtils
  //, contnrs
  , fgl;

type
  TCells = specialize TFPGMap<integer, string>;

procedure test_cells();

implementation

var cell_id: Integer;
var the_cells :TCells;

procedure test_cells();
//var h : TFPDataHashTable;
//var  test: string;
begin
  WriteLn('cells:test_cells()');

//  the_cells.add(cell_id, 'hello');
the_cells[cell_id] := 'hello';
end;

initialization
begin
  cell_id := 1;
  the_cells.create;
end;

end.

