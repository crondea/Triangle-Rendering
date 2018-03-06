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

#define VERTICIES_OFFSET 38
#define FACES_OFFSET 75
#define EXTRA_STUFF_OFFSET 55
#define MAX_SIZE 100
#define DEFAULT_DEPTH 9999999
#define ROW_SIZE 256
#define COL_SIZE ROW_SIZE


void findMinimums(float **verticies, int numVerts, float *mins);
void findMaximums(float **verticies, int numVerts, float *maxes);