/*
#pragma warning(disable:4996)
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <cmath>

#include <stdio.h>
#include <stdlib.h>
#define PI 3.141592

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
	float* dir_mag = (float*)calloc(9, sizeof(float));
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

			conv_x /= 9.0;
			conv_y /= 9.0;

			mag[y * width + x] = sqrt(conv_x * conv_x + conv_y * conv_y);
			dir = atan2(conv_y, conv_x);
			dir = dir * 180.0 / PI;
			if (dir < 0) dir = dir + 180.0;
			deg = dir / 20.0;
			dir_mag[deg] += mag[y * width + x];

			//just for visualization
			if (max < mag[y * width + x]) max = mag[y * width + x];
			if (min > mag[y * width + x]) min = mag[y * width + x];
		}
	}

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			imgEdge.at<uchar>(y, x) = 255 * (mag[y * width + x] - min) / (max - min);
		}
	}

	imwrite("edge.jpg", imgEdge);

	for (x = 0; x < 9; x++) {
		printf("dir_mag[%d] is %f\n", x, dir_mag[x] / (height * width));
	}
}

void main()
{
	int height, width;
	Mat imgGray = imread("test.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	height = imgGray.rows;
	width = imgGray.cols;

	gradient_computation(imgGray);
}
*/