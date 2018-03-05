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
    double cameraX, cameraY, cameraZ;
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
        cameraX = (double)(atoi(argv[2]));
        cameraY = (double)(atoi(argv[3]));
        cameraZ = (double)(atoi(argv[4]));
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
    double camera[3] = {1, 0, 0};
    double up[3] = {0, 0, 1};
    double Rx[9] = {1,0,0,0,cos(cameraX),-sin(cameraX),0,sin(cameraX),cos(cameraX)};
    double Ry[9] = {cos(cameraY),0,sin(cameraY),0,1,0,-sin(cameraY),0,cos(cameraY)};
    double Rz[9] = {cos(cameraZ),-sin(cameraZ),0,sin(cameraZ),cos(cameraZ),0,0,0,1};

    //      (b) Rotate both the camera vector and up vector by X degrees about
    //          the X-axis, Y degrees about the Y-axis, and Z degrees about the
    //          Z-axis, where X, Y and were supplied as command line args.
    camera[0] = (camera[0]*Rx[0] + camera[1]*Rx[3] + camera[2]*Rx[6]);
    camera[1] = (camera[0]*Rx[1] + camera[1]*Rx[4] + camera[2]*Rx[7]);
    camera[2] = (camera[0]*Rx[2] + camera[1]*Rx[5] + camera[2]*Rx[8]);

    camera[0] = (camera[0]*Ry[0] + camera[1]*Ry[3] + camera[2]*Ry[6]);
    camera[1] = (camera[0]*Ry[1] + camera[1]*Ry[4] + camera[2]*Ry[7]);
    camera[2] = (camera[0]*Ry[2] + camera[1]*Ry[5] + camera[2]*Ry[8]);

    camera[0] = (camera[0]*Rz[0] + camera[1]*Rz[3] + camera[2]*Rz[6]);
    camera[1] = (camera[0]*Rz[1] + camera[1]*Rz[4] + camera[2]*Rz[7]);
    camera[2] = (camera[0]*Rz[2] + camera[1]*Rz[5] + camera[2]*Rz[8]);

    //      (c) Move and scale the camera vector
    for(i = 0;i < 3; i++)
    {
        camera[i] = 1.5*E*camera[i] + center[i];
    }
    
    // 5. Determine the 3D coordinates bounding the image
    double centerMinusCamera[3] = {(center[0]-camera[0]),(center[1]-camera[1]),(center[2]-camera[2])};
    double left[3];
    double right[3];
    double top[3];
    double bottom[3];
    double topleft[3];
    left[0] = (up[1]*centerMinusCamera[2] - up[2]*centerMinusCamera[1]);
    left[1] = (up[2]*centerMinusCamera[0] - up[0]*centerMinusCamera[2]);
    left[2] = (up[0]*centerMinusCamera[1] - up[1]*centerMinusCamera[0]);
    
    double a = sqrt((left[0]*left[0]) + (left[1]*left[1]) + (left[2]*left[2]));

    left[0] = (E/(2*a))*left[0] + center[0];
    left[1] = (E/(2*a))*left[1] + center[1];
    left[2] = (E/(2*a))*left[2] + center[2];

    right[0] = (centerMinusCamera[1]*up[2] - centerMinusCamera[2]*up[1]);
    right[1] = (centerMinusCamera[2]*up[0] - centerMinusCamera[0]*up[2]);
    right[2] = (centerMinusCamera[0]*up[1] - centerMinusCamera[1]*up[0]);

    right[0] = (E/(2*a))*right[0] + center[0];
    right[1] = (E/(2*a))*right[1] + center[1];
    right[2] = (E/(2*a))*right[2] + center[2];

    top[0] = (E/2)*up[0] + center[0];
    top[1] = (E/2)*up[1] + center[1];
    top[2] = (E/2)*up[2] + center[2];

    bottom[0] = (-E/2)*up[0] + center[0];
    bottom[1] = (-E/2)*up[1] + center[1];
    bottom[2] = (-E/2)*up[2] + center[2];

    topleft[0] = (E/2)*up[0] + center[0];
    topleft[1] = (E/2)*up[1] + center[1];
    topleft[2] = (E/2)*up[2] + center[2];

    // 6. For each pixel r, c


    fclose(fptr);
    return 0;
}