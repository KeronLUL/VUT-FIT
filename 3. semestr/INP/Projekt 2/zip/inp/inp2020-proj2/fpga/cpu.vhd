-- cpu.vhd: Simple 8-bit CPU (BrainF*ck interpreter)
-- Copyright (C) 2020 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): Karel Norek
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet ROM
   CODE_ADDR : out std_logic_vector(11 downto 0); -- adresa do pameti
   CODE_DATA : in std_logic_vector(7 downto 0);   -- CODE_DATA <- rom[CODE_ADDR] pokud CODE_EN='1'
   CODE_EN   : out std_logic;                     -- povoleni cinnosti
   
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(9 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- ram[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_WE    : out std_logic;                    -- cteni (0) / zapis (1)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti 
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is

	---------------PC----------------
	signal pc_reg : std_logic_vector (11 downto 0);
	signal pc_inc : std_logic;
	signal pc_dec : std_logic;
	signal pc_ld  : std_logic;
	---------------------------------

	---------------RAS---------------
	type t_ras is array (0 to 15) of std_logic_vector (11 downto 0);
	signal ras_reg : t_ras := (others => X"000");
	signal ras_push: std_logic;
	signal ras_pop : std_logic;
	signal index: std_logic_vector(3 downto 0) := (others => '0');
	---------------------------------

	---------------CNT---------------
	signal cnt_reg : std_logic_vector (11 downto 0);
	signal cnt_inc : std_logic;
	signal cnt_dec : std_logic;
	---------------------------------

	---------------PTR---------------
	signal ptr_reg : std_logic_vector (9 downto 0);
	signal ptr_inc : std_logic;
	signal ptr_dec : std_logic;
	---------------------------------

	---------------MUX---------------
	signal mux_select : std_logic_vector (1 downto 0);
	---------------------------------

	---------------FSM---------------	
	type fsm_state is (
			s_idle, s_fetch, s_decode,
			s_ptr_inc, s_ptr_dec,
			s_val_inc0, s_val_inc1, s_val_inc2,
			s_val_dec0, s_val_dec1, s_val_dec2,
			s_while_start, s_while, s_while_loop0, s_while_loop1,
			s_while_end0, s_while_end1,
			s_print0, s_print1,
			s_get0, s_get1,
			s_null
		);

	signal pstate : fsm_state := s_idle;
	signal nstate : fsm_state;
	---------------------------------
begin
	---------------PC----------------
	pc_counter: process (RESET, CLK)
	begin
		if (RESET = '1') then
			pc_reg <= (others => '0');
		elsif (rising_edge(CLK)) then
			if (pc_ld = '1') then
				pc_reg <= ras_reg(conv_integer(index - 1));	
			elsif (pc_inc = '1') then
				pc_reg <= pc_reg + 1;
			elsif (pc_dec = '1') then
				pc_reg <= pc_reg - 1;
			end if;
		end if;
	end process;
	CODE_ADDR <= pc_reg;
	---------------------------------

	---------------RAS---------------
	ras_process: process (CLK)
	begin
		if (rising_edge(CLK)) then
			if (EN = '1') then
				if (ras_push = '1') then
					ras_reg(conv_integer(index)) <= pc_reg;
					index <= index + 1;
				elsif (ras_pop = '1') then
					ras_reg(conv_integer(index - 1)) <= (others => '0');
					index <= index - 1;
				end if;

			end if;
		end if;
	end process;
	---------------------------------

	---------------PTR---------------
	ptr_process: process (RESET, CLK)
	begin
		if (RESET = '1') then
			ptr_reg <= (others => '0');
		elsif (rising_edge(CLK)) then
			if (ptr_inc = '1') then
				ptr_reg <= ptr_reg + 1;
			elsif (ptr_dec = '1') then
				ptr_reg <= ptr_reg - 1;
			end if;
		end if;
	end process;
	DATA_ADDR <= ptr_reg;
	---------------------------------

	---------------CNT---------------
	cnt_counter: process (RESET, CLK)
	begin
		if (RESET = '1') then
			cnt_reg <= (others => '0');
		elsif (rising_edge(CLK)) then
			if (cnt_inc = '1') then
				cnt_reg <= cnt_reg + 1;
			elsif (cnt_dec = '1') then
				cnt_reg <= cnt_reg - 1;
			end if;
		end if;
	end process;
	---------------------------------

	---------------MUX---------------
	mux_process: process (RESET, CLK)
	begin
		if (RESET = '1') then
			DATA_WDATA <= (others => '0');	
		elsif (rising_edge(CLK)) then
			if (mux_select = "00") then
				DATA_WDATA <= IN_DATA;
			elsif (mux_select = "01") then
				DATA_WDATA <= DATA_RDATA + 1;
			elsif (mux_select = "10") then
				DATA_WDATA <= DATA_RDATA - 1;
			else 
				DATA_WDATA <= (others => '0');
			end if;
		end if;
	end process;
	---------------------------------

	OUT_DATA <= DATA_RDATA;

	---------------FSM---------------
	fsm_pstate_logic: process (RESET, CLK)
	begin
		if (RESET = '1') then
			pstate <= s_idle;
		elsif (rising_edge(CLK)) then
			if (EN = '1') then
				pstate <= nstate;
			end if;
		end if;
	end process;

	fsm_nstate_logic: process (pstate, IN_VLD, OUT_BUSY, CODE_DATA, DATA_RDATA, cnt_reg)
	begin
		CODE_EN <= '0';
		DATA_EN <= '0';
		DATA_WE <= '0';
		IN_REQ <= '0';
		OUT_WE <= '0';

		pc_inc <= '0';
		pc_dec <= '0';
		pc_ld <= '0';
		ras_pop <= '0';
		ras_push <= '0';
		cnt_inc <= '0';
		cnt_dec <= '0';
		ptr_inc <= '0';
		ptr_dec <= '0';

		mux_select <= "00";
			
		case pstate is
			when s_idle =>
				nstate <= s_fetch;

			when s_fetch => 
				CODE_EN <= '1';
				nstate <= s_decode;

			when s_decode =>
				case CODE_DATA is
					when X"3E" =>
						nstate <= s_ptr_inc;
					when X"3C" =>
						nstate <= s_ptr_dec;
					when X"2B" =>
						nstate <= s_val_inc0;
					when X"2D" =>
						nstate <= s_val_dec0;
					when X"5B" =>
						nstate <= s_while_start;
					when X"5D" =>
						nstate <= s_while_end0;
					when X"2E" =>
						nstate <= s_print0;
					when X"2C" =>
						nstate <= s_get0;
					when X"00" =>
						nstate <= s_null;
					when others =>
						pc_inc <= '1';
						nstate <= s_fetch;
				end case;
			-------null--------
			when s_null =>
				nstate <= s_null;
			-------- > --------
			when s_ptr_inc =>
				ptr_inc <= '1';
				pc_inc <= '1';
				nstate <= s_fetch;
			-------- < --------
			when s_ptr_dec =>
				ptr_dec <= '1';
				pc_inc <= '1';
				nstate <= s_fetch;
			-------- + --------
			when s_val_inc0 =>
				DATA_EN <= '1';
				DATA_WE <= '0';
				nstate <= s_val_inc1;
			when s_val_inc1 =>
				mux_select <= "01";
				nstate <= s_val_inc2;
			when s_val_inc2 =>
				DATA_EN <= '1';
				DATA_WE <= '1';
				pc_inc <= '1';
				nstate <= s_fetch;
			-------- - --------
			when s_val_dec0 =>
				DATA_EN <= '1';
				DATA_WE <= '0';
				nstate <= s_val_dec1;
			when s_val_dec1 =>
				mux_select <= "10";
				nstate <= s_val_dec2;
			when s_val_dec2 =>
				DATA_EN <= '1';
				DATA_WE <= '1';
				pc_inc <= '1';
				nstate <= s_fetch;
			-------- . --------
			when s_print0 =>
					DATA_EN <= '1';
					DATA_WE <= '0';
					nstate <= s_print1;
			when s_print1 =>
				if (OUT_BUSY = '1') then
					DATA_EN <= '1';
					DATA_WE <= '0';
					nstate <= s_print1;
				else
					OUT_WE <= '1';
					pc_inc <= '1';
					nstate <= s_fetch;
				end if;
			-------- , --------
			when s_get0 =>
				IN_REQ <= '1';
				mux_select <= "00";
				nstate <= s_get1;
			when s_get1 =>
				if (IN_VLD /= '1') then
					IN_REQ <= '1';
					mux_select <= "00";
					nstate <= s_get1;
				else
					DATA_EN <= '1';
					DATA_WE <= '1';
					pc_inc <= '1';
					nstate <= s_fetch;
				end if;
			-------- [ --------
			when s_while_start =>
				pc_inc <= '1';
				DATA_EN <= '1';
				DATA_WE <= '0';
				nstate <= s_while;
			when s_while =>
				if (DATA_RDATA = (DATA_RDATA'range => '0')) then
					CODE_EN <= '1';
					cnt_inc <= '1';
					nstate <= s_while_loop0;
				else
					ras_push <= '1';
					nstate <= s_fetch;
				end if;
			when s_while_loop0 =>
				if (cnt_reg /= (cnt_reg'range => '0')) then 
					if (CODE_DATA = X"5B") then
						cnt_inc <= '1';
					elsif (CODE_DATA = X"5D") then
						cnt_dec <= '1';
					end if;
					pc_inc <= '1';
					nstate <= s_while_loop1;
				else
					nstate <= s_fetch;
				end if;
			when s_while_loop1 =>
				CODE_EN <= '1';
				nstate <= s_while_loop0;
			-------- ] --------
			when s_while_end0 =>
				DATA_EN <= '1';
				DATA_WE <= '0';
				nstate <= s_while_end1;
			when s_while_end1 =>
				if (DATA_RDATA = (DATA_RDATA'range => '0')) then
					pc_inc <= '1';
					ras_pop <= '1';
				else
					pc_ld <= '1';
				end if;	
				nstate <= s_fetch;
			------OTHERS-------
			when others =>
				pc_inc <= '1';
				nstate <= s_fetch;
		end case;
	end process;
	---------------------------------
end behavioral;
 
