-- IVH Projekt podukol 1 - matrix_pack
-- Autor: Michal Pysik (login: xpysik00)
library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.STD_LOGIC_ARITH.all;
use IEEE.STD_LOGIC_SIGNED.all;


package matrix_pack is

-- Definice vyctoveho typu DIRECTION_T
TYPE DIRECTION_T is (DIR_LEFT, DIR_RIGHT);

-- Deklarace funkce GETID
function GETID (X, Y, ROWS, COLS : integer) return integer;

end matrix_pack;




package body matrix_pack is

-- Definice funkce GETID pro mapovani 2D souradnic na index v 1D poli
function GETID (X, Y, ROWS, COLS : integer) return integer is
	variable index, fixed_X, fixed_Y : integer; --index v 1D poli => navratova hodnota
begin
	fixed_X := X mod COLS;
	fixed_Y := Y mod ROWS;
	
	index := (fixed_X * ROWS) + fixed_Y;
	return index;
end;

end matrix_pack;
