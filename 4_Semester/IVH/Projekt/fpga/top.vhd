-- IVH PROJEKT FINAL
-- Autor: Michal Pysik (xpysik00)

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
use work.matrix_pack.all;


architecture main of tlv_gp_ifc is

	constant FPS : integer := 8; -- aktualne je nastaveno vykresleni 8 snimku za vterinu, kdykoli staci zmenit pouze tuto konstantu

  	signal A : std_logic_vector(3 downto 0) := (others => '0');
  	signal R : std_logic_vector(7 downto 0); 
	signal DATA : std_logic_vector(127 downto 0) := (others => '0'); -- display vzdy vykresluje tento vektor

	signal cell_DATA : std_logic_vector(127 downto 0) := (others => '0'); -- vystup bunek, pro prubehy 3x doleva a doprava je smerovan do DATA, pote je do DATA smerovana vlastni animace

	signal EN_frame : std_logic := '0'; -- povoleni vykreslit dalsi snimek, jinak je obrazek staticky

	signal CURR_DIR : DIRECTION_T := DIR_RIGHT;

	signal frame_counter : std_logic_vector(7 downto 0) := "00000000"; -- pocitadlo vykreslenych snimku pro kontroler

	TYPE image_matrix IS ARRAY(0 to 15) OF STD_LOGIC_VECTOR(7 downto 0); -- typ matice 16x8 s obrazkem
	constant image : image_matrix := ("00000000",
					  "00000000",
					  "00000000",
					  "00000000",
					  "00000000",
					  "00011100",
					  "01111110",
					  "01110010", -- obrazek ktery pujde 3x doprava a pak 3x doleva
					  "00010010",
					  "01110010",
					  "01111100",
					  "00000000",
					  "00000000",
					  "00000000",
					  "00000000",
					  "00000000");

	constant final_image_1 : std_logic_vector(127 downto 0) := "10111111" &
								   "00000000" &
								   "01111001" &
								   "01001001" &
								   "01001001" &
								   "01001111" &
								   "00000000" &
								   "01111110" & -- 1. ze 3 obrazku vlastni animace (SUS!)
								   "10000000" &
								   "10000000" &
								   "01111110" &
								   "00000000" &
								   "01111001" &
								   "01001001" &
								   "01001001" &
								   "01001111";

	constant final_image_2 : std_logic_vector(127 downto 0) := "00111100" &
								   "01000010" &
								   "01000010" &
								   "00111100" &
								   "00000000" &
								   "01111110" &
								   "00000100" &
								   "00001000" & -- 2. ze 3 obrazku vlastni animace (AMO)
								   "00000100" &
								   "01111110" &
								   "00000000" &
								   "01111000" &
								   "00010100" &
								   "00010010" &
								   "00010100" &
								   "01111000";

	constant final_image_3 : std_logic_vector(127 downto 0) := "01111010" &
								   "01001010" &
								   "01001010" &
								   "01001110" &
								   "00000000" &
								   "00111110" &
								   "01000000" &
								   "01000000" & -- 3. ze 3 obrazku vlastni animace (GUS)
								   "01000000" &
								   "00111110" &
								   "00000000" &
								   "00111010" &
								   "01001010" &
								   "01001010" &
								   "00100010" &
								   "00011100";

		
		

begin


	-- entita displaye, 100x za sekundu vykresli obrazek co se nachazi ve vektoru DATA
	dis : entity work.display port map (
		CLK => CLK,
		SMCLK => SMCLK,
		RESET => RESET,
		DATA => DATA,
		A => A,
		R => R

	);

    -- mapovani vystupu
    -- nemenit
    X(6) <= A(3);
    X(8) <= A(1);
    X(10) <= A(0);
    X(7) <= '0'; -- en_n
    X(9) <= A(2);

    X(16) <= R(1);
    X(18) <= R(0);
    X(20) <= R(7);
    X(22) <= R(2);
  
    X(17) <= R(4);
    X(19) <= R(3);
    X(21) <= R(6);
    X(23) <= R(5);




	-- instance counteru, aktivuje vykresleni dalsiho snimku (jeho frekvence udava FPS vysledneho "videa")
    	-- jeho vystupem je signal EN_frame, ktery jde jako input signal do vsech bunek (z jejich pohledu input signal EN)
	counter_frame : entity work.counter generic map(
		CLK_FREQ => 25000000,
 		OUT_FREQ =>      FPS  -- vysledne frames per second, konstanta FPS je definovana nahore
	) port map(
		CLK => CLK,
		RESET => RESET,
		EN => EN_frame -- EN vystup tohoto citace = EN_frame signal
	);	 


	-- vygenerujeme 128 bunek, nastavime vhodne vstupy a vystupy
	x_cell : for x in 0 to 15 generate
		y_cell : for y in 0 to 7 generate
			instance_cell : entity work.cell
			port map (
				CLK => CLK,
				RESET => RESET,
				INIT_STATE => image(x)(y),
				NEIGH_LEFT => cell_DATA( GETID(x-1, y, 8, 16) ),
				NEIGH_RIGHT => cell_DATA( GETID(x+1, y, 8, 16) ),
				DIRECTION => CURR_DIR,
				EN => EN_frame,
				STATE => cell_DATA( GETID(x, y, 8, 16) )
			);
		end generate;
	end generate;



	-- hlavni proces, ridi smer animace na prvnich 48 + 48 = 96 snimku, na dalsich 96 snimku ignoruje bunky a presmeruje do DATA vlastni animaci
	event_controller : process(CLK, RESET) is                                             -- tou se mysli 3 obrazky postupne, kazdy na 32 snimku
	begin
		if(RESET = '1') then
			CURR_DIR <= DIR_RIGHT;
			DATA <= cell_DATA;
			frame_counter <= "00000000";
		elsif(CLK = '1' and CLK'event) then
			if(EN_frame = '1') then
				if(frame_counter < 47) then  -- snimky 0 - 47 <= rotace doprava
					CURR_DIR <= DIR_RIGHT;
					DATA <= cell_data;
					frame_counter <= frame_counter + 1;
				elsif(frame_counter < 95) then  -- snimky 48 - 95 <= rotace doleva
					CURR_DIR <= DIR_LEFT;
					DATA <= cell_data;
					frame_counter <= frame_counter + 1;
				elsif(frame_counter < 127) then  -- snimky 96 - 127 <= prvni final obrazek
					CURR_DIR <= DIR_RIGHT; -- rotace ktere nevidime ho nechame delat doprava, pro plynulejsi prechod na konci cyklu
					DATA <= final_image_1;
					frame_counter <= frame_counter + 1;
				elsif(frame_counter < 159) then  -- snimky 128 - 159 <= druhy final obrazek
					DATA <= final_image_2;
					frame_counter <= frame_counter + 1;
				elsif(frame_counter < 191) then  -- snimky 160 - 191 <= treti final obrazek
					DATA <= final_image_3;
					frame_counter <= frame_counter + 1;
				else				-- pote se navratime na zacatek a smycka se opakuje
					DATA <= cell_DATA;
					frame_counter <= "00000000";
				end if;
			end if;
		end if;
	end process;




end main;

