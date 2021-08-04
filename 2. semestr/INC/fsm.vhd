-- fsm.vhd: Finite State Machine
-- Author(s): 
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
   type t_state is (TEST1, TEST2, TEST3, TEST4, TEST5, TEST6, TEST_A1, TEST_A2, 
							TEST_A3, TEST_B1, TEST_B2, TEST_B3, TEST_B4, FINALTEST, 
							ACCESS_DENIED, ACCESS_ALLOWED, FAILED, FINISH);
   signal present_state, next_state : t_state;

begin
-- -------------------------------------------------------
sync_logic : process(RESET, CLK)
begin
   if (RESET = '1') then
      present_state <= TEST1;
   elsif (CLK'event AND CLK = '1') then
      present_state <= next_state;
   end if;
end process sync_logic;

-- -------------------------------------------------------
next_state_logic : process(present_state, KEY, CNT_OF)
begin
   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST1 =>
      next_state <= TEST1;
		if (KEY(8) = '1') then
			next_state <= TEST2;
      elsif (KEY(15) = '1') then
         next_state <= ACCESS_DENIED;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= FAILED;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	when TEST2 =>
      next_state <= TEST2;
		if (KEY(7) = '1') then
			next_state <= TEST3;
      elsif (KEY(15) = '1') then
         next_state <= ACCESS_DENIED;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= FAILED;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	when TEST3 =>
      next_state <= TEST3;
		if (KEY(9) = '1') then
			next_state <= TEST4;
      elsif (KEY(15) = '1') then
         next_state <= ACCESS_DENIED;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= FAILED;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	when TEST4 =>
      next_state <= TEST4;
		if (KEY(8) = '1') then
			next_state <= TEST5;
      elsif (KEY(15) = '1') then
         next_state <= ACCESS_DENIED;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= FAILED;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	when TEST5 =>
      next_state <= TEST5;
		if (KEY(7) = '1') then
			next_state <= TEST6;
      elsif (KEY(15) = '1') then
         next_state <= ACCESS_DENIED;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= FAILED;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	when TEST6 =>
      next_state <= TEST6;
		if (KEY(2) = '1') then
			next_state <= TEST_A1;
		elsif (KEY(5) = '1') then
			next_state <= TEST_B1;
      elsif (KEY(15) = '1') then
         next_state <= ACCESS_DENIED;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= FAILED;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	when TEST_A1 =>
      next_state <= TEST_A1;
		if (KEY(3) = '1') then
			next_state <= TEST_A2;
      elsif (KEY(15) = '1') then
         next_state <= ACCESS_DENIED;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= FAILED;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	when TEST_A2 =>
      next_state <= TEST_A2;
		if (KEY(5) = '1') then
			next_state <= TEST_A3;
      elsif (KEY(15) = '1') then
         next_state <= ACCESS_DENIED;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= FAILED;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	when TEST_A3 =>
      next_state <= TEST_A3;
		if (KEY(0) = '1') then
			next_state <= FINALTEST;
      elsif (KEY(15) = '1') then
         next_state <= ACCESS_DENIED;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= FAILED;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	when TEST_B1 =>
      next_state <= TEST_B1;
		if (KEY(2) = '1') then
			next_state <= TEST_B2;
      elsif (KEY(15) = '1') then
         next_state <= ACCESS_DENIED;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= FAILED;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	when TEST_B2 =>
      next_state <= TEST_B2;
		if (KEY(5) = '1') then
			next_state <= TEST_B3;
      elsif (KEY(15) = '1') then
         next_state <= ACCESS_DENIED;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= FAILED;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	when TEST_B3 =>
      next_state <= TEST_B3;
		if (KEY(3) = '1') then
			next_state <= TEST_B4;
      elsif (KEY(15) = '1') then
         next_state <= ACCESS_DENIED;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= FAILED;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	when TEST_B4 =>
      next_state <= TEST_B4;
		if (KEY(3) = '1') then
			next_state <= FINALTEST;
      elsif (KEY(15) = '1') then
         next_state <= ACCESS_DENIED;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= FAILED;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	when FINALTEST =>
		next_state <= FINALTEST;
		if (KEY(15) = '1') then
			next_state <= ACCESS_ALLOWED;
		elsif (KEY(14 downto 0) /= "000000000000000") then
			next_state <= FAILED;
		end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
	when FAILED =>
      next_state <= FAILED;
      if (KEY(15) = '1') then
         next_state <= ACCESS_DENIED;
      end if;
	-- - - - - - - - - - - - - - - - - - - - - - -
	when ACCESS_ALLOWED =>
      next_state <= ACCESS_ALLOWED;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when ACCESS_DENIED =>
      next_state <= ACCESS_DENIED;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
      next_state <= FINISH;
      if (KEY(15) = '1') then
         next_state <= TEST1; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
      next_state <= TEST1;
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
   when ACCESS_DENIED =>
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
	when ACCESS_ALLOWED =>
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
		FSM_MX_MEM 		<= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
		if (KEY(14 downto 0) /= "000000000000000") then
			FSM_LCD_WR     <= '1';
      elsif (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   end case;
end process output_logic;

end architecture behavioral;

