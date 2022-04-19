-- IVH Projekt podukol 1 - testbench pro matrix_pack
-- Autor: Michal Pysik (login: xpysik00)
-- Tento soubor neni na syntezu, pouze na simulaci, v konzoli se vypisuje prubeh testu (konstrukci assert pouzivam i na vypis zprav o zacatku a konce testovani)
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_signed.all;
use ieee.std_logic_unsigned.all;
use ieee.std_logic_textio.all;
use ieee.numeric_std.all;
use std.textio.all;
use work.matrix_pack.all; -- muj balicek matrix_pack



----- Deklarace entity testbenche
entity matrix_pack_tb is
end entity matrix_pack_tb;



----- Deklarace architektury testbenche
architecture behavioral of matrix_pack_tb is

begin

	process
		variable result : integer;
	begin

		assert false report "(not error) Starting the tests...";

		wait for 100 ns;
		result := GETID(0, 0, 1, 1); -- X, Y, ROWS, COLS
		assert result = 0 report "First test failed (X = 0, Y = 0, ROWS = 1, COLS = 1, EXPECTED_RETVAL = 0)";

		wait for 100 ns;
		result := GETID(3, 7, 10, 6);
		assert result = 37 report "Second test failed (X = 3, Y = 7, ROWS = 10, COLS = 6,  EXPECTED_RETVAL = 37)";

		wait for 100 ns;
		result := GETID(-1, -1, 6, 5);
		assert result = 29 report "Third test failed (X = -1, Y = -1, ROWS = 6, COLS = 5,  EXPECTED_RETVAL = 29)";

		wait for 100 ns;
		result := GETID(-3, 10, 20, 20);
		assert result = 350 report "Fourth test failed (X = -3, Y = 10, ROWS = 20, COLS = 20,  EXPECTED_RETVAL = 350)";

		assert false report "(not error) All tests done. All failed tests are listed above this text (if there are any).";

		wait;
	end process;

end architecture behavioral;


