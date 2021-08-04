# Návrh počítačových systémů

### **Akademický rok 2020/2021**

## Projekt 1 - Vigenèrova šifra
**Hodnocení:** 15/15 <br>

V jazyce VHDL popište, programem Xilinx Isim odsimulujte a do binárního řetězce pro FPGA syntetizujte obvod realizující lehce modifikovaný algoritmus Vigenèrovy šifry. Vigenèrova šifra patří do kategorie substitučních šifer a její princip pro potřeby tohoto projektu bude spočívat v nahrazování každého znaku zprávy znakem, který je v abecedě posunut o hodnotu danou příslušným znakem šifrovacího klíče. Šifrování bude probíhat tak, že první znak klíče posouvá znak zprávy vpřed, druhý znak klíče posouvá znak zprávy vzad, číslice jsou nahrazovány znakem #. Pokud vychází posuv před písmeno A nebo za písmeno Z, uvažuje se cyklicky z opačného konce abecedy.

## Projekt 2 - Brainfuck interpreter
**Hodnocení:** 22/23 <br>

Cílem projektu je implementovat pomocí VHDL procesor, který bude schopen vykonávat program napsaný v jazyce BrainFuck.