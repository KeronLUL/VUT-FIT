# Operační systémy

### **Akademický rok 2019/2020 a 2020/2021**

## Projekt 1 - histogram velikostí souborů (Ak. rok 2019/2020)
**Hodnocení:** 15/15 <br>

Cílem úlohy je vytvořit skript, který prozkoumá adresář a vytvoří report o jeho obsahu. Předmětem rekurzivního zkoumání adresáře je počet souborů a adresářů a velikosti souborů. Výstupem skriptu je textový report.

### Použití: ./dirgraph.sh [-i REGEX] [-n] [DIR]

Pokud je zadán argument -i, tak se ignoruji složky a soubory s nazvem *REGEX*. <br>
Argument -n normalizuje histogram, aby se vlezl do terminálu. <br>
Argument *DIR* zadává zkoumaný adresář. Pokud není zadán bere se aktualní adresář. <br>

## Projekt 2 - Santa Claus problem (Ak. rok 2020/2021)
**Hodnocení:** 15/15 <br>

Implementujte v jazyce C modifikovaný synchronizační problém Santa Claus z [The Little Book of Semaphores](http://greenteapress.com/semaphores/LittleBookOfSemaphores.pdf).

Santa Claus spí ve své dílně na severním pólu a může být vzbuzen pouze: <br>
1. Ve chvíli, kdy jsou všichni jeho sobi zpět z letní dovolené.
2. Někteří z jeho skřítků mají problém s výrobou hraček a potřebují pomoci.

Aby se Santa Claus prospal, skřítci čekají potichu před dílnou. Ve chvíli, kdy čekají alespoň 3, tak první 3 z fronty najednou vstoupí do dílny. Ostatní skřítci, co potřebují pomoci musí čekat před dílnou až bude volno. Ve chvíli, kdy přijde poslední sob z dovolené je pomoc čekajícím skřítkům možné odložit. Santa dává na dveře dílny nápis „Vánoce – zavřeno“ a jde zapřahat soby do saní. Všichni skřítci, co čekají před dílnou ihned odcházejí na dovolenou. Ostatní skřítci odchází na dovolenou ve chvíli, kdy potřebují pomoc od Santy a zjistí, že je dílna zavřená.

### Použití:
Přeložit projekt pomocí `make`. <br>
`./proj2 number_of_elves number_of_raindeers elf_work_time raindeer_vacation`