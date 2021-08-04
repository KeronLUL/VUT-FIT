/*
    IZU 2. projekt
    Autor: Karel Norek
    Zadani 37
*/

odd_count([], 0).
odd_count([H|T], N):- 
    odd_count(T, N1), N is N1, 0 is mod(H, 2).
odd_count([H|T], N):- 
    odd_count(T, N1), N is N1+1, 1 is mod(H, 2).
uloha37(SEZNAM, N):- 
    odd_count(SEZNAM, N1), N >= N1.