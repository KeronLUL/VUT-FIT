/**
 * IZP 2nd project
 * Author: Karel Norek, xnorek01@stud.fit.vutbr.cz
 * Date: 25.11.2019
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define I_0 1e-12
#define U_T 25.8563e-3

//equation for Kirchhoff's first law
double bisection(double U_p, double U_0, double R){
    return ((I_0 * ((exp(U_p / U_T)) - 1)) - ((U_0 - U_p)) / R);
}

//function that calculates U_p (operating point of diode) using bisection
double diode(double U_0, double R, double eps){
    double min = 0;
    double max = U_0;
    double U_p;
    double iteration_check = 0;
    //loop until (MAX - MIN) is lower or equals eps or until max number of iterations is reached
    do{
        U_p = (max + min) / 2;
        //if a has different sign than U_p write U_p into b (bisection method)
        if (bisection (U_p, U_0, R) > 0){
            max = U_p;
        }else min = U_p;
        //if previous U_p equals current U_p end while because it can no longer be bisected
        if (fabs(iteration_check - U_p) <= eps){
            break;
        }
        iteration_check = U_p;
    }while (fabs(max - min) > eps);
    return U_p;
}

int main(int argc, char *argv[]) {
    //argument check for right number of arguments
    if (argc != 4){
        fprintf(stderr, "invalid arguments\n");
        return EXIT_FAILURE;
    }
    char *str;
    double arg[3];
    //convert argument into double and check if it is a valid argument
    for (int i = 0; i < 3; i++) {
        arg[i] = strtod(argv[i + 1], &str);
        if (*str != '\0' || argv[i + 1][0] == '\0' || arg[i] < 0 || arg[1] == 0) {
            fprintf(stderr, "invalid arguments\n");
            return EXIT_FAILURE;
        }
    }
    double U_p = diode(arg[0], arg[1], arg[2]);
    //calculate I_p (operating point of diode) using already known U_p from function diode
    double I_p = I_0 * ((exp(U_p / U_T)) - 1);
    printf("Up=%g V\nIp=%g A\n", U_p, I_p);

    return EXIT_SUCCESS;
}