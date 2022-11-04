/*
#pragma warning(disable:4996)
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <cmath>

#include <stdio.h>
#include <stdlib.h>
#define PI 3.141592
#define bl_size 16
#define quan 9

using namespace cv;

void gradient_computation(Mat input)
{
	int x, y, xx, yy;
	int height = input.rows;
	int width = input.cols;
	float conv_x, conv_y;
	float dir;
	int deg;
	float max = -1.0;
	float min = 10000000.0;
	float* mag = (float*)calloc(height * width, sizeof(float));
	float* o_dir = (float*)calloc(height * width, sizeof(float));
	Mat imgEdge(height, width, CV_8UC1);
	int mask_x[9] = { -1,-1,-1,0,0,0,1,1,1 };
	int mask_y[9] = { -1,0,1,-1,0,1,-1,0,1 };

	//compute fx and fy
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			conv_x = 0;
			conv_y = 0;

			//inner product
			for (yy = y - 1; yy <= y + 1; yy++) {
				for (xx = x - 1; xx <= x + 1; xx++) {
					if (yy >= 0 && yy < height && xx >= 0 && xx < width) {
						conv_x += input.at<uchar>(yy, xx) * mask_x[(yy - (y - 1)) * 3 + (xx - (x - 1))];
						conv_y += input.at<uchar>(yy, xx) * mask_y[(yy - (y - 1)) * 3 + (xx - (x - 1))];
					}
				}
			}

			mag[y * width + x] = sqrt(conv_x * conv_x + conv_y * conv_y);
			dir = atan2(conv_y, conv_x);
			dir = dir * 180.0 / PI;
			o_dir[y * width + x] = dir;

			if (max < mag[y * width + x]) max = mag[y * width + x];
			if (min > mag[y * width + x]) min = mag[y * width + x];
		}
	}
	int size = ((width - 16) / 8 + 1) * ((height - 16) / 8 + 1);
	float** dir_mag = (float**)calloc(size, sizeof(float*));
	int idx_x = 0;
	for (x = 0; x < size; x++) {
		dir_mag[x] = (float*)calloc(quan, sizeof(float));
	}
	for (y = 0; y <= height - bl_size; y += bl_size/2) {
		for (x = 0; x <= width - bl_size; x += bl_size / 2) {
			for (yy = y; yy < y + bl_size; yy++) {
				for (xx = x; xx < x + bl_size; xx++) {
					deg = o_dir[yy * width + xx] / 20.0;
					dir_mag[idx_x][deg] += mag[yy * width + xx];
				}
			}
			idx_x++;
		}
	}

	FILE* fp = fopen("HOG3.csv", "w");
	fprintf(fp, "xy,compare2.bmp\n");
	float normal_pow;
	for (x = 0; x < size; x++) {
		normal_pow = 0.0;
		for (y = 0; y < quan; y++) {
			normal_pow += (dir_mag[x][y] * dir_mag[x][y]);
		}
		for (y = 0; y < quan; y++) {
			dir_mag[x][y] = dir_mag[x][y] / (sqrt(normal_pow)+0.0000001);
			fprintf(fp, "%d%d,%f\n", x, y, dir_mag[x][y]);
		}
	}
	fclose(fp);
}

void main()
{
	int height, width;
	Mat imgGray = imread("compare2.bmp", CV_LOAD_IMAGE_GRAYSCALE);
	height = imgGray.rows;
	width = imgGray.cols;

	gradient_computation(imgGray);
}
*/