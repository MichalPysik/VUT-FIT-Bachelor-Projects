-- Blikani ledkami
-- IVH Projekt 3 : display
-- Michal Pysik (xpysik00)
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;


-- DO entity nezasahujte, bude pouzita k simulaci!
entity display is
	port (
		data : in std_logic_vector(127 downto 0); -- data k zobrazeni, format je na popsany dole
		reset : in std_logic;
		clk : in std_logic; -- hodiny 25 MHz
		smclk : in std_logic; -- hodiny 7.3728MHz
		A : out std_logic_vector(3 downto 0);
		R : out std_logic_vector(7 downto 0)
	);
end entity display;

architecture behv of display is
   constant TMR : natural := 20; -- 18 je normalni

   signal cnt : std_logic_vector(23 downto 0) := (others => '0');
   signal A_temp : std_logic_vector(3 downto 0) := (others => '0');
   signal R_temp : std_logic_vector(7 downto 0); 

   signal EN : std_logic;
	-- definujte si libovolne signaly
begin

-- Vystupni logika. Vas ukol: vse smazat a naimplementovat zobrazeni matice z vektoru data
-- prvek 0 ukazuje 0. radek a 0. sloupec
-- prvek 1 ukazuje 0. radek a 1. sloupec
-- prvek 127 ukazuje na 7. radek a 15. sloupec

-- cilem je to, aby byl text citelny!


	-- instance synchronniho citace z druheho podukolu
	counter_instance : entity work.counter generic map(
		CLK_FREQ => 25000000,
 		OUT_FREQ =>     1600
	) port map(
		CLK => CLK,
		RESET => RESET,
		EN => EN
	);	



	-- proces radice displaye
	process(CLK, RESET) is
		variable LSB : integer range 0 to 120 := 0; -- musim pouzit integer, indexovani pres std_vector nefunguje
	begin
		if(RESET = '1') then
			LSB := 0;
			A_temp <= "0000";
			R_temp <= data(7 downto 0);
		elsif(CLK = '1' and CLK'event) then
			if(EN = '1') then
				if(LSB = 120) then
					LSB := 0;
				else
					LSB := LSB + 8;
				end if;
				A_temp <= A_temp + 1;
				R_temp <= data((LSB + 7) downto LSB);
			end if;
		end if;
	end process;


	A <= A_temp;
	R <= R_temp;

end behv;

