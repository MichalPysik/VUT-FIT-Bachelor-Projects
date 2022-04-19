-- fsm.vhd: Finite State Machine
-- Author(s): Michal Pysik (xpysik00)
--
library ieee;
use ieee.std_logic_1164.all;
-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity fsm is
port(
   CLK         : in  std_logic;
   RESET       : in  std_logic;

   -- Input signals
   KEY         : in  std_logic_vector(15 downto 0);
   CNT_OF      : in  std_logic;

   -- Output signals
   FSM_CNT_CE  : out std_logic;
   FSM_MX_MEM  : out std_logic;
   FSM_MX_LCD  : out std_logic;
   FSM_LCD_WR  : out std_logic;
   FSM_LCD_CLR : out std_logic
);
end entity fsm;

-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of fsm is
   type t_state is (START, LR0, L1, L2, L3, L4, L5, L6,                 --- L reprezentuje posloupnost prvniho a R druheho spravneho kodu
			L7, L8, L9, R1 ,R2, R3,										--- LR0 maji spolecny jelikoz oba zacinaji cislem 1
			R4, R5, R6, R7, R8, LRX,									--- LRX je jejich opet spolecny spravny konec, ktery jen ceka na znak # pro potvrzeni, nebo se v pripade jineho znaku prepne na spatny kod -> WRONG
			WRONG, ALLOWED, DENIED, FINISH);							--- WRONG je kdyz je posloupnost spatne, je jedno kolik a jakych dalsich znaku uzivatel zada, stejne se po stisku # prepne machine do DENIED
   signal present_state, next_state : t_state;

begin
-- -------------------------------------------------------
sync_logic : process(RESET, CLK)
begin
   if (RESET = '1') then
      present_state <= START;
   elsif (CLK'event AND CLK = '1') then
      present_state <= next_state;
   end if;
end process sync_logic;

-- -------------------------------------------------------
next_state_logic : process(present_state, KEY, CNT_OF)
begin
   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when START =>
	if (KEY(1) = '1') then
		next_state <= LR0;
	elsif (KEY(15) = '1') then
		next_state <= DENIED;
	elsif (KEY(14 downto 0) /= "000000000000000") then
		next_state <= WRONG;
	else
		next_state <= START;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when WRONG =>
	if (KEY(15) = '1') then
		next_state <= DENIED;
	else
		next_state <= WRONG;
	end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when LR0 =>
	if (KEY(3) = '1') then
		next_state <= L1;
	elsif (KEY(6) = '1') then
		next_state <= R1;
	elsif (KEY(15) = '1') then
		next_state <= DENIED;
	elsif (KEY(14 downto 0) /= "000000000000000") then
		next_state <= WRONG;
	else
		next_state <= LR0;
	end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when L1 =>
	if (KEY(8) = '1') then
		next_state <= L2;
	elsif (KEY(15) = '1') then
		next_state <= DENIED;
	elsif (KEY(14 downto 0) /= "000000000000000") then
		next_state <= WRONG;
	else
		next_state <= L1;
	end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
      when L2 =>
	if (KEY(0) = '1') then
		next_state <= L3;
	elsif (KEY(15) = '1') then
		next_state <= DENIED;
	elsif (KEY(14 downto 0) /= "000000000000000") then
		next_state <= WRONG;
	else
		next_state <= L2;
	end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
      when L3 =>
	if (KEY(9) = '1') then
		next_state <= L4;
	elsif (KEY(15) = '1') then
		next_state <= DENIED;
	elsif (KEY(14 downto 0) /= "000000000000000") then
		next_state <= WRONG;
	else
		next_state <= L3;
	end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when L4 =>
	if (KEY(7) = '1') then
		next_state <= L5;
	elsif (KEY(15) = '1') then
		next_state <= DENIED;
	elsif (KEY(14 downto 0) /= "000000000000000") then
		next_state <= WRONG;
	else
		next_state <= L4;
	end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when L5 =>
	if (KEY(9) = '1') then
		next_state <= L6;
	elsif (KEY(15) = '1') then
		next_state <= DENIED;
	elsif (KEY(14 downto 0) /= "000000000000000") then
		next_state <= WRONG;
	else
		next_state <= L5;
	end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when L6 =>
	if (KEY(0) = '1') then
		next_state <= L7;
	elsif (KEY(15) = '1') then
		next_state <= DENIED;
	elsif (KEY(14 downto 0) /= "000000000000000") then
		next_state <= WRONG;
	else
		next_state <= L6;
	end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when L7 =>
	if (KEY(9) = '1') then
		next_state <= L8;
	elsif (KEY(15) = '1') then
		next_state <= DENIED;
	elsif (KEY(14 downto 0) /= "000000000000000") then
		next_state <= WRONG;
	else
		next_state <= L7;
	end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when L8 =>
	if (KEY(7) = '1') then
		next_state <= L9;
	elsif (KEY(15) = '1') then
		next_state <= DENIED;
	elsif (KEY(14 downto 0) /= "000000000000000") then
		next_state <= WRONG;
	else
		next_state <= L8;
	end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when L9 =>
	if (KEY(6) = '1') then
		next_state <= LRX;
	elsif (KEY(15) = '1') then
		next_state <= DENIED;
	elsif (KEY(14 downto 0) /= "000000000000000") then
		next_state <= WRONG;
	else
		next_state <= L9;
	end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when R1 =>
	if (KEY(0) = '1') then
		next_state <= R2;
	elsif (KEY(15) = '1') then
		next_state <= DENIED;
	elsif (KEY(14 downto 0) /= "000000000000000") then
		next_state <= WRONG;
	else
		next_state <= R1;
	end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
      when R2 =>
	if (KEY(3) = '1') then
		next_state <= R3;
	elsif (KEY(15) = '1') then
		next_state <= DENIED;
	elsif (KEY(14 downto 0) /= "000000000000000") then
		next_state <= WRONG;
	else
		next_state <= R2;
	end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
      when R3 =>
	if (KEY(2) = '1') then
		next_state <= R4;
	elsif (KEY(15) = '1') then
		next_state <= DENIED;
	elsif (KEY(14 downto 0) /= "000000000000000") then
		next_state <= WRONG;
	else
		next_state <= R3;
	end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when R4 =>
	if (KEY(6) = '1') then
		next_state <= R5;
	elsif (KEY(15) = '1') then
		next_state <= DENIED;
	elsif (KEY(14 downto 0) /= "000000000000000") then
		next_state <= WRONG;
	else
		next_state <= R4;
	end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when R5 =>
	if (KEY(3) = '1') then
		next_state <= R6;
	elsif (KEY(15) = '1') then
		next_state <= DENIED;
	elsif (KEY(14 downto 0) /= "000000000000000") then
		next_state <= WRONG;
	else
		next_state <= R5;
	end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when R6 =>
	if (KEY(6) = '1') then
		next_state <= R7;
	elsif (KEY(15) = '1') then
		next_state <= DENIED;
	elsif (KEY(14 downto 0) /= "000000000000000") then
		next_state <= WRONG;
	else
		next_state <= R6;
	end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when R7 =>
	if (KEY(5) = '1') then
		next_state <= R8;
	elsif (KEY(15) = '1') then
		next_state <= DENIED;
	elsif (KEY(14 downto 0) /= "000000000000000") then
		next_state <= WRONG;
	else
		next_state <= R7;
	end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when R8 =>
	if (KEY(8) = '1') then
		next_state <= LRX;
	elsif (KEY(15) = '1') then
		next_state <= DENIED;
	elsif (KEY(14 downto 0) /= "000000000000000") then
		next_state <= WRONG;
	else
		next_state <= R8;
	end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when LRX =>
	if (KEY(15) = '1') then
		next_state <= ALLOWED;
	elsif (KEY(14 downto 0) /= "000000000000000") then
		next_state <= WRONG;
	else
		next_state <= LRX;
	end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when ALLOWED =>
	if (CNT_OF = '1') then
		next_state <= FINISH;
	else
		next_state <= ALLOWED;
	end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when DENIED =>
	if (CNT_OF = '1') then
		next_state <= FINISH;
	else
		next_state <= DENIED;
	end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
	if (KEY(15) = '1') then
		next_state <= START;
	else
		next_state <= FINISH;
	end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   end case;
end process next_state_logic;

-- -------------------------------------------------------
output_logic : process(present_state, KEY)
begin
   FSM_CNT_CE     <= '0';
   FSM_MX_MEM     <= '0';
   FSM_MX_LCD     <= '0';
   FSM_LCD_WR     <= '0';
   FSM_LCD_CLR    <= '0';

   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when ALLOWED =>
		FSM_CNT_CE	<= '1';
		FSM_MX_MEM	<= '1';
		FSM_MX_LCD	<= '1';
		FSM_LCD_WR	<= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
   when DENIED =>
		FSM_CNT_CE	<= '1';
		FSM_MX_LCD	<= '1';
		FSM_LCD_WR	<= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
		if (KEY(14 downto 0) /= "000000000000000") then
			FSM_LCD_WR <= '1';
		elsif (KEY(15) = '1') then
			FSM_LCD_CLR <= '1';
		end if;
   end case;
end process output_logic;

end architecture behavioral;

