proc isim_script {} {

   add_divider "Signals of the Vigenere Interface"
   add_wave_label "" "CLK" /testbench/clk
   add_wave_label "" "RST" /testbench/rst
   add_wave_label "-radix ascii" "DATA" /testbench/tb_data
   add_wave_label "-radix ascii" "KEY" /testbench/tb_key
   add_wave_label "-radix ascii" "CODE" /testbench/tb_code

   add_divider "Vigenere Inner Signals"
   
   add_wave_label "-radix unsigned" "SHIFT" /testbench/uut/shift
   add_wave_label "-radix ascii" "ADD" /testbench/uut/shiftAdded
   add_wave_label "-radix ascii" "DEC" /testbench/uut/shiftDecreased
   
   add_wave_label "" "state" /testbench/uut/state
   add_wave_label "-radix unsigned" "next state" /testbench/uut/nextState
   add_wave_label "" "FSMout" /testbench/uut/FSMout
   run 8 ns
}
