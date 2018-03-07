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
    FILE *optr = NULL;
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

        if(!(optr = fopen("output.ppm","wb")))
        {
            printf("Error opening output file\n");
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
    float tempCam[3],tempUp[3];
    tempCam[0] = camera[0];
    tempCam[1] = camera[1];
    tempCam[2] = camera[2];
    tempUp[0] = up[0];
    tempUp[1] = up[1];
    tempUp[2] = up[2];
    for(i = 0; i < 3; i++)
    {  
        camera[i] = (tempCam[0]*Rx[i]) + (tempCam[1]*Rx[i+3]) + (tempCam[2]*Rx[i+6]); 
        up[i] = (tempUp[0] * Rx[i]) + (tempUp[1]*Rx[i+3]) + (tempUp[2]*Rx[i+6]);
    }
    tempCam[0] = camera[0];
    tempCam[1] = camera[1];
    tempCam[2] = camera[2];
    tempUp[0] = up[0];
    tempUp[1] = up[1];
    tempUp[2] = up[2];
    for(i = 0; i < 3; i++)
    {  
        camera[i] = (tempCam[0]*Ry[i]) + (tempCam[1]*Ry[i+3]) + (tempCam[2]*Ry[i+6]);
        up[i] = (tempUp[0] * Ry[i]) + (tempUp[1]*Ry[i+3]) + (tempUp[2]*Ry[i+6]);

    }
    tempCam[0] = camera[0];
    tempCam[1] = camera[1];
    tempCam[2] = camera[2];
    tempUp[0] = up[0];
    tempUp[1] = up[1];
    tempUp[2] = up[2];
    for(i = 0; i < 3; i++)
    {  
        camera[i] = (tempCam[0]*Rz[i]) + (tempCam[1]*Rz[i+3]) + (tempCam[2]*Rz[i+6]);
        up[i] = (tempUp[0] * Rz[i]) + (tempUp[1]*Rz[i+3]) + (tempUp[2]*Rz[i+6]);
    }

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
    // printf("a        %f E %f \nup %f %f %f \nleft %f %f %f\n",a,E,up[0],up[1],up[2],left[0],left[1],left[2]);

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
    float vZero[3], vOne[3], vTwo[3];
    float vOneMinusvZero[3], vTwoMinusvZero[3];
    float n,d;
    float intersect[3];
    float intersectMinusvZero[3],intersectMinusvOne[3],intersectMinusvTwo[3];
    float vTwoMinusvOne[3],vZeroMinusvOne[3],vZeroMinusvTwo[3],vOneMinusvTwo[3];
    float cross1[3],cross2[3];
    float dot1,dot2,dot3;
    unsigned char color[ROWS][COLS];
    unsigned char maxColor = 0;

    for(i = 0; i < ROWS; i++)
    {
        for(j = 0; j < COLS; j++)
        {
            zBuffer = DEFAULT_DEPTH;
            //  (c) Calculate vector coordinates <image>
            for(k = 0; k < 3; k++)
            {
                image[i][j][k] = topleft[k] 
                                + (((float)j/(COLS-1))*(right[k]-left[k])) 
                                + (((float)i/(ROWS-1))*(bottom[k]-top[k]));
                
            }
            //if(i == 1 && j == 1)printf("image: %f %f %f\n",image[i][j][0],image[i][j][1],image[i][j][2]);

            //  (d) For each triangle having coordinates v0, v1, and v2
            for(k = 0; k < faceCount; k++)
            {
                //    (i) Fine the plane equation <A,B,C,D> that contains the trianlge.
                vZero[0] = verts[faces[k][1]][0];
                vZero[1] = verts[faces[k][1]][1];
                vZero[2] = verts[faces[k][1]][2];
                vOne[0] = verts[faces[k][2]][0];
                vOne[1] = verts[faces[k][2]][1];
                vOne[2] = verts[faces[k][2]][2];
                vTwo[0] = verts[faces[k][3]][0];
                vTwo[1] = verts[faces[k][3]][1];
                vTwo[2] = verts[faces[k][3]][2];
                
                vOneMinusvZero[0] = vOne[0] - vZero[0];
                vOneMinusvZero[1] = vOne[1] - vZero[1];
                vOneMinusvZero[2] = vOne[2] - vZero[2];
                vTwoMinusvZero[0] = vTwo[0] - vZero[0];
                vTwoMinusvZero[1] = vTwo[1] - vZero[1];
                vTwoMinusvZero[2] = vTwo[2] - vZero[2];

                plane[i][j][0] = (vOneMinusvZero[1]*vTwoMinusvZero[2] - vOneMinusvZero[2]*vTwoMinusvZero[1]);
                plane[i][j][1] = (vOneMinusvZero[2]*vTwoMinusvZero[0] - vOneMinusvZero[0]*vTwoMinusvZero[2]);
                plane[i][j][2] = (vOneMinusvZero[0]*vTwoMinusvZero[1] - vOneMinusvZero[1]*vTwoMinusvZero[0]);
                plane[i][j][3] = -(plane[i][j][0]*vZero[0] + plane[i][j][1]*vZero[1] + plane[i][j][2]*vZero[2]);

                //    (ii) Find the distance along the image pixel ray to the triangle.
                //         Test if ray is parallel to triangle, skip this triangle if so.
                n = -(plane[i][j][0]*camera[0] + plane[i][j][1]*camera[1] + plane[i][j][2]*camera[2]) 
                        - plane[i][j][3];
                d = (plane[i][j][0]*(image[i][j][0] - camera[0]) + plane[i][j][1]*(image[i][j][1] - camera[1]) 
                        + plane[i][j][2]*(image[i][j][2] - camera[2]));
                
                //    (iii) Find the 3D coordinates <intersect> of ray and plane.
                intersect[0] = camera[0] + (n / d)*(image[i][j][0] - camera[0]);
                intersect[1] = camera[1] + (n / d)*(image[i][j][1] - camera[1]);
                intersect[2] = camera[2] + (n / d)*(image[i][j][2] - camera[2]);

                //    (iv) Determine if intersection point lies within triangle by 
                //         calculating the three dot products.
                vTwoMinusvOne[0] = vTwo[0] - vOne[0];
                vTwoMinusvOne[1] = vTwo[1] - vOne[1];
                vTwoMinusvOne[2] = vTwo[2] - vOne[2];
                vZeroMinusvOne[0] = vZero[0] - vOne[0];
                vZeroMinusvOne[1] = vZero[1] - vOne[1];
                vZeroMinusvOne[2] = vZero[2] - vOne[2];
                vZeroMinusvTwo[0] = vZero[0] - vTwo[0];
                vZeroMinusvTwo[1] = vZero[1] - vTwo[1];
                vZeroMinusvTwo[2] = vZero[2] - vTwo[2];
                vOneMinusvTwo[0] = vOne[0] - vTwo[0];
                vOneMinusvTwo[1] = vOne[1] - vTwo[1];
                vOneMinusvTwo[2] = vOne[2] - vTwo[2];
                intersectMinusvZero[0] = intersect[0] - vZero[0];
                intersectMinusvZero[1] = intersect[1] - vZero[1];
                intersectMinusvZero[2] = intersect[2] - vZero[2];

                cross1[0] = (vTwoMinusvZero[1]*vOneMinusvZero[2] - vTwoMinusvZero[2]*vOneMinusvZero[1]);
                cross1[1] = (vTwoMinusvZero[2]*vOneMinusvZero[0] - vTwoMinusvZero[0]*vOneMinusvZero[2]);
                cross1[2] = (vTwoMinusvZero[0]*vOneMinusvZero[1] - vTwoMinusvZero[1]*vOneMinusvZero[0]);
                cross2[0] = (intersectMinusvZero[1]*vOneMinusvZero[2] - intersectMinusvZero[2]*vOneMinusvZero[1]);
                cross2[1] = (intersectMinusvZero[2]*vOneMinusvZero[0] - intersectMinusvZero[0]*vOneMinusvZero[2]);
                cross2[2] = (intersectMinusvZero[0]*vOneMinusvZero[1] - intersectMinusvZero[1]*vOneMinusvZero[0]);
                dot1 = cross1[0]*cross2[0] + cross1[1]*cross2[1] + cross1[2]*cross2[2];

                cross1[0] = (vZeroMinusvOne[1]*vTwoMinusvOne[2] - vZeroMinusvOne[2]*vTwoMinusvOne[1]);
                cross1[1] = (vZeroMinusvOne[2]*vTwoMinusvOne[0] - vZeroMinusvOne[0]*vTwoMinusvOne[2]);
                cross1[2] = (vZeroMinusvOne[0]*vTwoMinusvOne[1] - vZeroMinusvOne[1]*vTwoMinusvOne[0]);
                cross2[0] = (intersectMinusvOne[1]*vTwoMinusvOne[2] - intersectMinusvOne[2]*vTwoMinusvOne[1]);
                cross2[1] = (intersectMinusvOne[2]*vTwoMinusvOne[0] - intersectMinusvOne[0]*vTwoMinusvOne[2]);
                cross2[2] = (intersectMinusvOne[0]*vTwoMinusvOne[1] - intersectMinusvOne[1]*vTwoMinusvOne[0]);
                dot2 = cross1[0]*cross2[0] + cross1[1]*cross2[1] + cross1[2]*cross2[2];

                cross1[0] = (vOneMinusvTwo[1]*vZeroMinusvTwo[2] - vOneMinusvTwo[2]*vOneMinusvTwo[1]);
                cross1[1] = (vOneMinusvTwo[2]*vZeroMinusvTwo[0] - vOneMinusvTwo[0]*vOneMinusvTwo[2]);
                cross1[2] = (vOneMinusvTwo[0]*vZeroMinusvTwo[1] - vOneMinusvTwo[1]*vOneMinusvTwo[0]);
                cross2[0] = (intersectMinusvTwo[1]*vZeroMinusvTwo[2] - intersectMinusvTwo[2]*vZeroMinusvTwo[1]);
                cross2[1] = (intersectMinusvTwo[2]*vZeroMinusvTwo[0] - intersectMinusvTwo[0]*vZeroMinusvTwo[2]);
                cross2[2] = (intersectMinusvTwo[0]*vZeroMinusvTwo[1] - intersectMinusvTwo[1]*vZeroMinusvTwo[0]);
                dot3 = cross1[0]*cross2[0] + cross1[1]*cross2[1] + cross1[2]*cross2[2];

                //if(i == 1 && j == 1 && k == 1) printf("dot1:%f dot2: %f dot3: %f\n",dot1,dot2,dot3);
                
                //    (v) If any of the dot products is < 0, then the intersection point
                //        lies outside the triangle and it can be skipped.
                if(dot1 < 0 || dot2 < 0 || dot3 < 0)
                {
                    // Skip
                    continue;
                }

                //    (vi) If the distance to the triangle n/d is greater than the current
                //         z-buffer value for this pixel, then the trianlge list behind a
                //         closer triangle and it can be skipped.
                if((n / d) > zBuffer)
                {
                    // Skip
                    continue;
                }

                //    (vii) Set pixel color to 155 + (i%100) where i is the index of the
                //          triangle.
                color[i][j] = 155 + (k % 100);
                if(color[i][j] > maxColor) maxColor = color[i][j];
                zBuffer = n / d;
            }
        }
    }

    // 7. Write PPM image
    fprintf(optr,"%s\n","P5");
    fprintf(optr,"%d %d\n",ROWS,COLS);
    fprintf(optr,"%d\n",maxColor);
    for(i = 0; i < ROWS; i++)
    {
        for(j = 0; j < COLS; j++)
        {
            fwrite(&color[i][j],sizeof(unsigned char),1,optr);
        }
    }


    fclose(fptr);
    fclose(optr);
    return 0;
}