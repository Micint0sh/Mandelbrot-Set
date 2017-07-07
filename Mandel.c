#include <stdio.h>
#include "qdbmp.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>

#define ITERLIMIT 1000
#define LENGLIMIT 2
#define WIDOFIMAGE 1024
#define HEIOFIMAGE 1024

int main()
{
	//Create blank image
	BMP* image = BMP_Create(WIDOFIMAGE, HEIOFIMAGE, 24);
	//Set initial center and zoom value
	double centerX;
	double centerY;
	double zoom;
	printf("CenterX:\n");
	scanf("%lf", &centerX);
	printf("\nCenterY:\n");
	scanf("%lf", &centerY);
	printf("\nZoom:\n");
	scanf("%lf", &zoom);
	//Reduce zoom by 4 to match the coordinates
	zoom = zoom / 4;
	//Trick for faster calculation
	double reducedZoom = 1 / (2 * zoom);
	double reducedWid = 1 / ((double)WIDOFIMAGE*zoom);
	double reducedHei = 1 / ((double)HEIOFIMAGE*zoom);
	//Malloc array for storing iteration values of each pixel
	int* iters = malloc(sizeof(int)*WIDOFIMAGE*HEIOFIMAGE);
	memset(iters, 0, sizeof(int)*WIDOFIMAGE*HEIOFIMAGE);
	//Malloc array for histogram
	int* histo = malloc(sizeof(int)*ITERLIMIT);
	memset(histo, 0, sizeof(int)*ITERLIMIT);
	//Iteration starts
	//For every pixel
	for (int i = 0; i < WIDOFIMAGE; i++)
	{
		for (int j = 0; j < HEIOFIMAGE; j++)
		{
			//Calculating coordinates of c in complex plane
			double coorX = i * reducedWid - reducedZoom + centerX;
			double coorY = j * reducedHei - reducedZoom + centerY;
			//Starts from Z0=0
			double zX = 0, zY = 0;
			int iter = 0;
			//Testing if it will converge
			while (1)
			{
				//Calculate the real parts and imaginary parts separately
				double zXtemp = zX*zX - zY*zY + coorX;
				zY = 2 * zX*zY + coorY;
				zX = zXtemp;
				//If it goes out of lengthlimit, it is going to blow up
				if (zX*zX + zY*zY >= LENGLIMIT*LENGLIMIT) //If blows up
				{
					//Set the corresponding pixel's iter value
					iters[j*WIDOFIMAGE + i] = iter;
					//Add one to the pixel counter in histogram
					histo[iter]++;
					break;
				}
				if (iter >= ITERLIMIT) //Converges
				{
					//Set -1 means in the Mandelbrot set
					iters[j*WIDOFIMAGE + i] = -1;
					break;
				}
				iter++;
			}
		}
	}
	//Preparing balancing light values
	int total = 0;
	//Count total iteration amount
	for (int i = 0; i < ITERLIMIT; i++)
	{
		total += histo[i];
	}
	//Fill in colors
	for (int i = 0; i < WIDOFIMAGE; i++)
	{
		for (int j = 0; j < HEIOFIMAGE; j++)
		{
			double value = 0;
			//Find how this pixel's iter value is ranked among all pixels
			for (int l = 0; l < iters[j*WIDOFIMAGE + i]; l++)
			{
				value += (double)histo[l];
			}
			value = value / total;
			//Re-calibrating color
			value = pow(value, 4);
			value *= 255;
			//Set color (B&W)
			BMP_SetPixelRGB(image, i, j, value, value, value);
		}
	}
	//Automatically generate filename
	char file[150] = "C:\\Users\\HP\\Documents\\Visual Studio 2015\\Projects\\Mandelbrot_Plot_C\\Plots\\";
	int len = strlen(file);
	//Writing X coor
	file[len] = 'X';
	file[len + 1] = 0;
	char* tmplen=malloc(sizeof(char) * 50);
	sprintf(tmplen, "%.5f", centerX);
	strcat(file, tmplen);
	//Writing Y coor
	len = strlen(file);
	file[len] = 'Y';
	file[len + 1] = 0;
	memset(tmplen, 0, 50 * sizeof(char));
	sprintf(tmplen, "%.5f", centerY);
	strcat(file, tmplen);
	//Writing Zoom
	len = strlen(file);
	file[len] = 'Z';
	file[len + 1] = 0;
	memset(tmplen, 0, 50 * sizeof(char));
	sprintf(tmplen, "%.2f", zoom*4);
	strcat(file, tmplen);
	//Write .bmp
	len = strlen(file);
	file[len] = '.';
	file[len + 1] = 'b';
	file[len + 2] = 'm';
	file[len + 3] = 'p';
	file[len + 4] = 0;
	//Write image
	BMP_WriteFile(image, file);
	BMP_Free(image);
	free(tmplen);
	return 0;
}
