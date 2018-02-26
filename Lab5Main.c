/*  Corey Rondeau
 *  ECE 4680 Lab 5
 *  File Created: Feb. 26 2018
 *  File Name: Lab5Main.c
 * 
 */


#include "render.h"


int main(int argc, char *argv[])
{
    FILE *fptr = NULL;

    if(argc != 2)
    {
        printf("Usage: filename [input file]");
        exit(-1);
    }
    else
    {
        if(!(fptr = fopen(argv[1],"rb")))
        {
            printf("Error opening input file");
            exit(-1);
        }
    }


    fclose(fptr);
    return 0;
}