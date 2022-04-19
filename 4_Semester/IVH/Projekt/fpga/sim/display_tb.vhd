-- Display test bench
-- IVH projekt - ukol 3
-- autor: Michal Pysik (xpysik00)
-- SIMULACE MUSI BEZET ALESPON 10 ms!!!!

LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE work.display;


-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
USE ieee.numeric_std.ALL;
 
ENTITY display_tb IS
END display_tb;


ARCHITECTURE behavior OF display_tb IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT display	
    PORT(
         CLK : IN  std_logic;
    	 SMCLK : IN std_logic;
         RESET : IN  std_logic;
         data : IN  std_logic_vector(127 downto 0);
    	 R : OUT  std_logic_vector(7 downto 0);
    	 A : OUT  std_logic_vector(3 downto 0)
        );
    END COMPONENT;
    

   --Inputs
   signal CLK : std_logic := '0';
   signal RESET : std_logic := '0';
   signal SMCLK : std_logic := '0';
   signal data : std_logic_vector(127 downto 0) := 	
   		"00000000" & -- Moje inicialy stejne jako v top.vhd
		"00000110" &
		"00001001" &
		"00001001" &
		"00001001" &
		"11111111" &
		"00000000" &
		"00000000" &
		"11111111" &
		"00000010" &
		"00000100" &
		"00001000" &
		"00000100" &
		"00000010" &
		"11111111" &
		"00000000";
 	--Outputs
   signal R : std_logic_vector(7 downto 0);
   signal A : std_logic_vector(3 downto 0);

   type arr_16x8 is array(0 to 15) of std_logic_vector(7 downto 0);  -- 16 12-bit adres ... 16*12 = 192 bitu celkem
   signal virtual_display : arr_16x8;

   -- Clock period definitions
   constant CLK_period : time := 40 ns; -- aby to odpovidalo 25 MHz
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
	uut: entity work.display port map (
		CLK => CLK,
		SMCLK => SMCLK,
		RESET => RESET,
		A => A,
		R => R,
		data => data
	);
	
   -- Clock process definitions
   CLK_process: process
   begin
		CLK <= '0';
		wait for CLK_period/2;
		CLK <= '1';
		wait for CLK_period/2;
   end process;
 

   -- Stimulus process -- SIMULACE MUSI BEZET ALESPON 10 ms!!!
   stim_proc: process
   begin				-- postupne budeme naplnovat virtualni display (16x8 array) virtual_display
					-- na konci testbenche by mel zobrazit me inicialy " M P "	
		
		RESET <= '1';
		wait for 125 ns;
		RESET <= '0';

		virtual_display(0) <= R;
		wait for 625 us;

		virtual_display(1) <= R;
		wait for 625 us;

		virtual_display(2) <= R;
		wait for 625 us;

		virtual_display(3) <= R;
		wait for 625 us;

		virtual_display(4) <= R;
		wait for 625 us;

		virtual_display(5) <= R;
		wait for 625 us;

		virtual_display(6) <= R;
		wait for 625 us;

		virtual_display(7) <= R;
		wait for 625 us;

		virtual_display(8) <= R;
		wait for 625 us;

		virtual_display(9) <= R;
		wait for 625 us;

		virtual_display(10) <= R;
		wait for 625 us;

		virtual_display(11) <= R;
		wait for 625 us;

		virtual_display(12) <= R;
		wait for 625 us;

		virtual_display(13) <= R;
		wait for 625 us;

		virtual_display(14) <= R;
		wait for 625 us;

		virtual_display(15) <= R;


      wait;
   end process;

END;
