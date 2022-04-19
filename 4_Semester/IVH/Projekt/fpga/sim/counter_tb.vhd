-- Citac s volitelnou frekvenci test bench
-- IVH projekt - ukol2
-- autor: Michal Pysik (xpysik00) 

LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE work.counter;


-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
USE ieee.numeric_std.ALL;
 
ENTITY counter_tb IS
END counter_tb;


ARCHITECTURE behavior OF counter_tb IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT counter
	 Generic (
			CLK_FREQ : positive := 100000;
			OUT_FREQ : positive := 10000);	
    PORT(
         CLK : IN  std_logic;
         RESET : IN  std_logic;
         EN : OUT  std_logic
        );
    END COMPONENT;
    

   --Inputs
   signal CLK : std_logic := '0';
   signal RESET : std_logic := '0';

 	--Outputs
   signal EN : std_logic;

   -- Clock period definitions
   constant CLK_period : time := 1 ns;
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
	-- vas ukol !!!!!!!!! vcetne nastaveni generickych parametru
	uut: entity work.counter generic map (CLK_FREQ => 1_000_000,
														OUT_FREQ => 50_000) -- 1. perioda, 21. perioda, ... kazda 20. perioda ma spustit EN
										port map (CLK => CLK,
														RESET => RESET,
														EN => EN);
	
   -- Clock process definitions
   CLK_process: process
   begin
		CLK <= '0';
		wait for CLK_period/2;
		CLK <= '1';
		wait for CLK_period/2;
   end process;
 

   -- Stimulus process
   stim_proc: process
   begin
		RESET <= '1';
		wait for 10 ps;
		assert EN = '0' report "EN has to be 0 while RESET is active";
		wait for 10 ns;
		RESET <= '0';

		wait for 1 ns;
		assert EN = '1' report "First clock period after RESET ended";
		wait for 1 ns;
		assert EN = '0' report "One clock period after first check since reset";

		wait for 17 ns;
		assert EN = '0' report "18 clock periods after first check since reset";

		wait for 2ns;
		assert EN = '1' report "20 clock periods after first check since reset";
		
		wait for 20 ns;
		assert EN = '1' report "40 clock periods after first check since reset";
		
		wait for 10 ns;
		assert EN = '0' report "50 clock periods after first check since reset";
		
		wait for 10 ns;
		assert EN = '1' report "60 clock periods after first check since reset";
	
      -- vas ukol !!!!!!!!! nastavte spravne reset, spuste pocitani

      wait;
   end process;

END;
