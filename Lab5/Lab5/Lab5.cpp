// Lab5.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{
	FILE *fptr = NULL;




	if (!(fptr = fopen(argv[1], "rb")))
	{
		printf("Error opening input file!");
		exit(-1);
	}





    return 0;
}

