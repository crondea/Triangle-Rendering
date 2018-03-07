/*  Corey Rondeau
 *  ECE 4680 Lab 5
 *  File Created: Feb. 26 2018
 *  File Name: Lab5Main.c
 * 
 */


#include "render.h"


int main(int argc, char *argv[])
{
    //FILE *fptr = NULL;
    float cameraX, cameraY, cameraZ;
    char buffer[MAX_SIZE];
    int i,j,k;
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
        cameraX = (float)(atoi(argv[2]) * M_PI / 180);
        cameraY = (float)(atoi(argv[3]) * M_PI / 180);
        cameraZ = (float)(atoi(argv[4]) * M_PI / 180);
    }

    // 1. Parse the PLY file header to determine the number of verticies and faces. --------
    fread(buffer, sizeof(unsigned char),VERTICIES_OFFSET,fptr);
    fscanf(fptr,"%d",&vertCount);
    fread(buffer, sizeof(unsigned char),FACES_OFFSET,fptr);
    fscanf(fptr,"%d",&faceCount);
    fread(buffer, sizeof(unsigned char),EXTRA_STUFF_OFFSET,fptr);

    // 2. Read the PLY file verticies and faces --------------------------------------------
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
    //   (a) Minimum and maximum X, Y and Z (two vectors <min> and <max>)
    float minXYZ[3];
    findMinimums(verts, vertCount, minXYZ);
    float maxXYZ[3];
    findMaximums(verts, vertCount, maxXYZ);

    // (b) Center X, Y, and Z (vector denoted <center>)
    float center[3];
    center[0] = maxXYZ[0] - ((maxXYZ[0] - minXYZ[0]) / 2.0);
    center[1] = maxXYZ[1] - ((maxXYZ[1] - minXYZ[1]) / 2.0);
    center[2] = maxXYZ[2] - ((maxXYZ[2] - minXYZ[2]) / 2.0);

    //  (c) Maximum extent of bounding box E = scalar that is largest component
    //      of (max-min), i.e. the largest extent of the three axes
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
    //    and <up>
    //    (a) By default assume <camera> is <1,0,0> with <up> oriented as 
    //    <0,0,1>.
    float camera[3] = {1, 0, 0};
    float up[3] = {0, 0, 1};
    float Rx[9] = {1,0,0,0,cos(cameraX),-sin(cameraX),0,sin(cameraX),cos(cameraX)};
    float Ry[9] = {cos(cameraY),0,sin(cameraY),0,1,0,-sin(cameraY),0,cos(cameraY)};
    float Rz[9] = {cos(cameraZ),-sin(cameraZ),0,sin(cameraZ),cos(cameraZ),0,0,0,1};

    //  (b) Rotate both the camera vector and up vector by X degrees about
    //      the X-axis, Y degrees about the Y-axis, and Z degrees about the
    //      Z-axis, where X, Y and were supplied as command line args.
    float temp[3];
    temp[0] = camera[0];
    temp[1] = camera[1];
    temp[2] = camera[2];
    for(i = 0; i < 3; i++)
    {  
        camera[i] = (temp[0]*Rx[i]) + (temp[1]*Rx[i+3]) + (temp[2]*Rx[i+6]); 
    }
    temp[0] = camera[0];
    temp[1] = camera[1];
    temp[2] = camera[2];
    for(i = 0; i < 3; i++)
    {  
        camera[i] = (temp[0]*Ry[i]) + (temp[1]*Ry[i+3]) + (temp[2]*Ry[i+6]);
    }
    temp[0] = camera[0];
    temp[1] = camera[1];
    temp[2] = camera[2];
    for(i = 0; i < 3; i++)
    {  
        camera[i] = (temp[0]*Rz[i]) + (temp[1]*Rz[i+3]) + (temp[2]*Rz[i+6]);
    }
    printf("camera: %f %f %f\n",camera[0],camera[1],camera[2]);


    // (c) Move and scale the camera vector
    for(i = 0; i < 3; i++)
    {
        camera[i] = 1.5*E*camera[i] + center[i];
    }

    /*** UP TO HERE IS CHECKED AND CORRECT ***/
    
    // 5. Determine the 3D coordinates bounding the image
    float centerMinusCamera[3] = {(center[0]-camera[0]),(center[1]-camera[1]),(center[2]-camera[2])};
    float left[3];
    float right[3];
    float top[3];
    float bottom[3];
    float topleft[3];

    left[0] = (up[1]*centerMinusCamera[2] - up[2]*centerMinusCamera[1]);
    left[1] = (up[2]*centerMinusCamera[0] - up[0]*centerMinusCamera[2]);
    left[2] = (up[0]*centerMinusCamera[1] - up[1]*centerMinusCamera[0]);
    
    float a = sqrt((left[0]*left[0]) + (left[1]*left[1]) + (left[2]*left[2]));

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

    topleft[0] = (E/2)*up[0] + left[0];
    topleft[1] = (E/2)*up[1] + left[1];
    topleft[2] = (E/2)*up[2] + left[2];

    // printf("left:    %f %f %f\n",left[0],left[1],left[2]);
    // printf("right:   %f %f %f\n",right[0],right[1],right[2]);
    // printf("top:     %f %f %f\n",top[0],top[1],top[2]);
    // printf("bottom:  %f %f %f\n",bottom[0],bottom[1],bottom[2]);
    // printf("topleft: %f %f %f\n",topleft[0],topleft[1],topleft[2]);



    // 6. For each pixel r, c
    //  (a) Default image color is black (greyscale = 0)
    //  (b) Default z-buffer depth is very far
    float zBuffer;
    float image[ROWS][COLS][3];
    float plane[ROWS][COLS][4];

    for(i = 0; i < 256; i++)
    {
        for(j = 0; j < 256; j++)
        {
            zBuffer = DEFAULT_DEPTH;
            //  (c) Calculate vector coordinates <image>
            for(k = 0; k < 3; k++)
            {
                image[i][j][k] = topleft[k] 
                                + ((j/(COLS-1))*(right[k]-left[k]))
                                + ((i/(ROWS-1))*(bottom[k]-top[k]));
            }

            //  (d) For each triangle having coordinates v0, v1, and v2
            //    (i) Fine the plane equation <A,B,C,D> that contains the trianlge.
            for(k = 0; k < 3; k++)
            {
                //plane[i][j][k] = 
            }
            //plane[i][j][3] = -();
            //    (ii) Find the distance along the image pixel ray to the triangle.
            //         Test if ray is parallel to triangle, skip this triangle if so.

            //    (iii) Find the 3D coordinates <intersect> of ray and plane.

            //    (iv) Determine if intersection point lies within triangle by 
            //         calculating the three dot products.

            //    (v) If any of the dot products is < 0, then the intersection point
            //        lies outside the triangle and it can be skipped.

            //    (vi) If the distance to the triangle n/d is greater than the current
            //         z-buffer value for this pixel, then the trianlge list behind a
            //         closer triangle and it can be skipped.

            //    (vii) Set pixel color to 155 + (i%100) where i is the index of the
            //          triangle.

        }
    }






    fclose(fptr);
    return 0;
}