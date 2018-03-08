/*  Corey Rondeau
 *  ECE 4680 Lab 5
 *  File Created: Feb. 26 2018
 *  File Name: render.h
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define VERTICIES_OFFSET 36
#define FACES_OFFSET 71
#define EXTRA_STUFF_OFFSET 52
#define MAX_SIZE 100
#define DEFAULT_DEPTH 9999999
#define ROWS 256
#define COLS 256


void findMinimums(float **verticies, int numVerts, float *mins);
void findMaximums(float **verticies, int numVerts, float *maxes);