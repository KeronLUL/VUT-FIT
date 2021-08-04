library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

-- rozhrani Vigenerovy sifry
entity vigenere is
   port(
         CLK : in std_logic;
         RST : in std_logic;
         DATA : in std_logic_vector(7 downto 0);
         KEY : in std_logic_vector(7 downto 0);

         CODE : out std_logic_vector(7 downto 0)
    );
end vigenere;

-- V souboru fpga/sim/tb.vhd naleznete testbench, do ktereho si doplnte
-- znaky vaseho loginu (velkymi pismeny) a znaky klice dle vaseho prijmeni.

architecture behavioral of vigenere is
	-------------------SIGNALS FOR SHIFTS------------------
	signal shift: std_logic_vector(7 downto 0);
	signal shiftAdded: std_logic_vector(7 downto 0);
	signal shiftDecreased: std_logic_vector(7 downto 0);

	-------------------SIGNALS FOR MEALY-------------------
	type FSMstate is (add, dec);
	signal state: FSMstate;
	signal nextState: FSMstate;
	
	signal FSMout: std_logic_vector(1 downto 0); 
begin
	--------------------SHIFT PROCESSES--------------------
	shiftProcess: process (DATA, KEY) is
	begin
		shift <= KEY - 64;
	end process;

	shiftCalc: process (DATA, shift) is
		variable correction: std_logic_vector(7 downto 0);
	begin
		correction := "00011010";
		
		if (DATA + shift > 90) then
			shiftAdded <= DATA + shift - correction;
		else shiftAdded <= DATA + shift;
		end if;

		if (DATA - shift < 65) then
			shiftDecreased <= DATA - shift + correction;
		else shiftDecreased <= DATA - shift;
		end if;
	end process;

	-----------------------FSM MEALY-----------------------
	presentState: process (CLK, RST, DATA) is
	begin
		if (RST = '1') then
			state <= add;
		elsif rising_edge(CLK) then
			state <= nextState;
		end if;
	end process;

	nState: process (state, DATA, RST) is
	begin
		nextState <= add;
		
		if (state = add) then
			nextState <= dec;
			FSMout <= "01";
		elsif (state = dec) then
			nextState <= add;
			FSMout <= "10";
		end if;

		if (DATA < 58 and DATA > 47) then
			FSMout <= "00";
		elsif (RST = '1') then
			FSMout <= "00";
		end if;
	end process;
	
	------------------------MULTIPLEXOR--------------------
	multiplexor: process (FSMout, shiftAdded, shiftDecreased) is
		variable hashtag: std_logic_vector(7 downto 0);
	begin
		hashtag := "00100011";
		if (FSMout = "01") then
			CODE <= shiftAdded;
		elsif (FSMout = "10") then
			CODE <= shiftDecreased;
		elsif (FSMout = "00") then
			CODE <= hashtag;
		end if;
	end process;

end behavioral;
