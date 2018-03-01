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
    int cameraX, cameraY, cameraZ;
    char buffer[MAX_SIZE];
    int i;
    int vertCount;
    int faceCount;

    if(argc != 5)
    {
        printf("Usage: filename <input file> <x> <y> <z>\n");
        exit(-1);
    }
    else
    {
        if(!(fptr = fopen(argv[1],"rb")))
        {
            printf("Error opening input file\n");
            exit(-1);
        }
        cameraX = atoi(argv[2]);
        cameraY = atoi(argv[3]);
        cameraZ = atoi(argv[4]);
    }

    // 1. Parse the PLY file header to determine the number of verticies and faces. --------
    fread(buffer, sizeof(unsigned char),VERTICIES_OFFSET,fptr);
    fscanf(fptr,"%d",&vertCount);
    fread(buffer, sizeof(unsigned char),FACES_OFFSET,fptr);
    fscanf(fptr,"%d",&faceCount);
    fread(buffer, sizeof(unsigned char),EXTRA_STUFF_OFFSET,fptr);

    // 2. Read the PLY file verticies and faces --------------------------------------------
    //float verts[vertCount][3];
    float **verts = (float **)calloc(vertCount,sizeof(float*));
    for(i = 0; i < vertCount; i++)
    {
        verts[i] = (float *)calloc(3,sizeof(float));
        fscanf(fptr,"%f %f %f", &verts[i][0], &verts[i][1], &verts[i][2]);
    }
    int **faces = (int **)calloc(faceCount,sizeof(int*));
    for(i = 0; i < faceCount; i++)
    {
        faces[i] = (int *)calloc(4,sizeof(int));
        fscanf(fptr,"%d %d %d %d", &faces[i][0], &faces[i][1], &faces[i][2], &faces[i][3]);
    }

    // 3. Calculate the bounding box on the verticies. -------------------------------------
    //      (a) Minimum and maximum X, Y and Z (two vectors <min> and <max>)
    float minXYZ[3];
    findMinimums(verts, vertCount, minXYZ);
    float maxXYZ[3];
    findMaximums(verts, vertCount, maxXYZ);

    //      (b) Center X, Y, and Z (vector denoted <center>)
    float center[3];
    center[0] = maxXYZ[0] - ((maxXYZ[0] - minXYZ[0]) / 2.0);
    center[1] = maxXYZ[1] - ((maxXYZ[1] - minXYZ[1]) / 2.0);
    center[2] = maxXYZ[2] - ((maxXYZ[2] - minXYZ[2]) / 2.0);

    //      (c) Maximum extent of bounding box E = scalar that is largest component
    //          of (max-min), i.e. the largest extent of the three axes
    float E;
    float tempMax = maxXYZ[0] - minXYZ[0];
    for(i = 0; i < 3; i++)
    {
        if((maxXYZ[i] - minXYZ[i]) > tempMax)
        {
            tempMax = (maxXYZ[i] - minXYZ[i]);
        }
    }
    E = tempMax;
    
    // 4. Calculate the camera position and orientation using 2 vectors <camera>
    //      and <up>
    //      (a) By default assume <camera> is <1,0,0> with <up> oriented as 
    //          <0,0,1>.
    int camera[3] = {1, 0, 0};
    int up[3] = {0, 0, 1};
    //      (b) Rotate both the camera vector and up vector by X degrees about
    //          the X-axis, Y degrees about the Y-axis, and Z degrees about the
    //          Z-axis, where X, Y and were supplied as command line args.
    

    fclose(fptr);
    return 0;
}