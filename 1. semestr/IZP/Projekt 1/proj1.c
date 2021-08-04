/**
 * IZP First project
 * Author: Karel Norek, xnorek01@stud.fit.vutbr.cz
 * Date: 11.10.2019
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 102

//use if until you find a match than convert it into number
int use_if (int buf_char, int buf_number) {
    //if buf_char is character else if buf_char is number
    if (buf_char == 'a' || buf_char == 'b' || buf_char == 'c') {
        buf_number = '2';
    } else if (buf_char == 'd' || buf_char == 'e' || buf_char == 'f') {
        buf_number = '3';
    } else if (buf_char == 'g' || buf_char == 'h' || buf_char == 'i') {
        buf_number = '4';
    } else if (buf_char == 'j' || buf_char == 'k' || buf_char == 'l') {
        buf_number = '5';
    } else if (buf_char == 'm' || buf_char == 'n' || buf_char == 'o') {
        buf_number = '6';
    } else if (buf_char == 'p' || buf_char == 'q' || buf_char == 'r' || buf_char == 's') {
        buf_number = '7';
    } else if (buf_char == 't' || buf_char == 'u' || buf_char == 'v') {
        buf_number = '8';
    } else if (buf_char == 'w' || buf_char == 'x' || buf_char == 'y' || buf_char == 'z') {
        buf_number = '9';
    } else if (buf_char == '+') {
        buf_number = '0';
    //if buf_char is a number
    } else if (buf_char >= '0' && buf_char <= '9') {
        buf_number = buf_char;
    } else if (!isdigit(buf_char) && !isalpha(buf_char)) {
        buf_number = '\0';
    }
    return buf_number;
}

//compare argument with stdin
int compare (char buf[MAX], char *argv[]) {
    unsigned buf_len = strlen(buf);
    unsigned arg_len = strlen(argv[1]);
    unsigned breakpoint = 0;
    unsigned char_counter = 0;
    int counter = 0;
    //loop until the whole argument is checked
    for (unsigned j = 0; j < arg_len ; j++) {
        int char_arg = argv[1][j];
        //loop until the whole buffer is checked
        for (unsigned i = breakpoint ; i < buf_len; i++) {
            int buf_char;
            int match = 0;
            int buf_number = '\0';
            //if character is uppercase turn it into lowercase
            buf_char = tolower(buf[i]);
            //compare argument with buffer (with converted characters into numbers done by function use_if)
            if (use_if(buf_char, buf_number) == char_arg) {
                char_counter++;
                match++;
            }
            //if match is found make a breakpoint where the loop should resume
            if (match != 0) {
                breakpoint = i + 1;
                break;
            }
        }
        //compare counter with length of argument
        if (arg_len == char_counter) {
            counter++;
        }
    }
    return counter;
}

//loop until every line form stdin is read
int loop (int argc, char *argv[]) {
    int counter = 0;
    char name[MAX];
    char number[MAX];
    //loop until fgets reach the end and read stdin
    while (fgets(name, MAX, stdin) != NULL && (fgets(number, MAX, stdin)) != NULL) {
        //if no argument is set print the whole list else compare number and name, if match was found add +1 to counter
        if (argc == 1) {
            fprintf(stdout, "%.*s, %s", (unsigned) strlen(name) - 1, name, number);
            counter = 1;
        }else if (compare(number, argv) != 0 || compare(name, argv) != 0) {
            fprintf(stdout, "%.*s, %s", (unsigned) strlen(name) - 1, name, number);
            counter++;
        }
    }
    return counter;
}

//argument check
unsigned argument_check(int argc, char *argv[]){
    if (argc > 2) {
        fprintf(stderr, "wrong argument\n");
        return 1;
    }else if (argc == 2) {
        unsigned arg_len = strlen(argv[1]);
        for (unsigned i = 0; i < arg_len; i++) {
            if (!isdigit(argv[1][i])) {
                fprintf(stderr, "wrong argument\n");
                return 1;
            }
        }
    }
    return 0;
}

int main (int argc, char *argv[]) {
    //argument check
    if ((argument_check(argc, argv)) == 1){
        return EXIT_FAILURE;
    }
    //if nothing was found print Not found
    if ((loop(argc, argv)) == 0) {
        fprintf(stdout, "Not found\n");
    }
    return EXIT_SUCCESS;
}