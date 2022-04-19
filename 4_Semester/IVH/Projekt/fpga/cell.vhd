-- IVH Projekt 4 - entita Cell (bunka)
-- Autor: Michal Pysik (xpysik00)

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
use work.matrix_pack.all;


entity cell is -- entita bunka, celkem jich bude 128
	Port (
		CLK : in STD_LOGIC;
		RESET : in STD_LOGIC;
		INIT_STATE : in STD_LOGIC; -- puvodni stav
		NEIGH_LEFT : in STD_LOGIC; -- levy soused
		NEIGH_RIGHT : in STD_LOGIC; -- pravy soused
		DIRECTION : in DIRECTION_T; -- jde bud doprava, nebo doleva
		EN : in STD_LOGIC; -- povoleni posunu
		STATE : out STD_LOGIC -- vystup => bud sviti, nebo nesviti
	);
end cell;





architecture behavioral of cell is

	signal CURRENT_STATE : std_logic := '0'; -- pred prvnim resetem se nic nevypise

begin

	process (CLK, RESET) is
	begin
		if(RESET = '1') then
			CURRENT_STATE <= INIT_STATE;
		elsif(CLK = '1' and CLK'event) then
			if(EN = '1') then
				if(DIRECTION = DIR_RIGHT) then
					CURRENT_STATE <= NEIGH_LEFT;
				elsif(DIRECTION = DIR_LEFT) then
					CURRENT_STATE <= NEIGH_RIGHT;
				end if;
			end if;
		end if;
	end process;


	STATE <= CURRENT_STATE;


end behavioral;
		
		


