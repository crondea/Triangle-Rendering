/*  Corey Rondeau
 *  ECE 4680 Lab 5
 *  File Created: Feb. 26 2018
 *  File Name: render.c
 * 
 */

#include "render.h"


void findMinimums(float **verticies, int numVerts, float *mins)
{
    int i;
    mins[0] = verticies[0][0];
    mins[1] = verticies[0][1];
    mins[2] = verticies[0][2];

    // Loop through all of the verticies
    for(i = 1; i < numVerts; i++)
    {
        if(verticies[i][0] < mins[0])
        {
            mins[0] = verticies[i][0];
        }
        if(verticies[i][1] < mins[1])
        {
            mins[1] = verticies[i][1];
        }
        if(verticies[i][2] < mins[2])
        {
            mins[2] = verticies[i][2];
        }
    }
}

void findMaximums(float **verticies, int numVerts, float *maxes)
{
    int i;
    maxes[0] = verticies[0][0];
    maxes[1] = verticies[0][1];
    maxes[2] = verticies[0][2];

    // Loop through all of the verticies
    for(i = 1; i < numVerts; i++)
    {
        if(verticies[i][0] > maxes[0])
        {
            maxes[0] = verticies[i][0];
        }
        if(verticies[i][1] > maxes[1])
        {
            maxes[1] = verticies[i][1];
        }
        if(verticies[i][2] > maxes[2])
        {
            maxes[2] = verticies[i][2];
        }
    }
}