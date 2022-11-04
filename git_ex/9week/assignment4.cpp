#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <cmath>

#include <stdio.h>
#include <stdlib.h>
#define PI 3.141592
#define bl_size 12
#define quan 9

using namespace cv;

void ref_HOG(Mat input, float* output)
{
	int x, y, xx, yy;
	int height = input.rows;
	int width = input.cols;
	float conv_x, conv_y;
	float dir;
	int deg;
	float* mag = (float*)calloc(height * width, sizeof(float));
	float* o_dir = (float*)calloc(height * width, sizeof(float));
	int mask_y[9] = { -1,-1,-1,0,0,0,1,1,1 };
	int mask_x[9] = { -1,0,1,-1,0,1,-1,0,1 };

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
			if (dir < 0) dir += 180;
			o_dir[y * width + x] = dir;
		}
	}
	int idx = 0;
	float normal_pow;
	for (y = 0; y <= height - bl_size; y += bl_size / 2) {
		for (x = 0; x <= width - bl_size; x += bl_size / 2) {
			for (yy = y; yy < y + bl_size; yy++) {
				for (xx = x; xx < x + bl_size; xx++) {
					deg = o_dir[yy * width + xx] / 20.0;
					output[idx*quan+deg] += mag[yy * width + xx];
				}
			}
			idx++;
		}
	}
	for (x = 0; x < idx; x++) {
		normal_pow = 0.0;
		for (y = 0; y < quan; y++) {
			normal_pow += (output[x*quan+y] * output[x * quan + y]);
		}
		for (y = 0; y < quan; y++) {
			output[x * quan + y] = output[x * quan + y] / (sqrt(normal_pow));
			if (normal_pow == 0.0)
				output[x * quan + y] = 0.0;
		}
	}
}

void tar_HOG(Mat input, float* output, int pos_y, int pos_x, int w_height, int w_width)
{
	int x, y, xx, yy;
	float conv_x, conv_y;
	float dir;
	int deg;
	float* mag = (float*)calloc(w_height * w_width, sizeof(float));
	float* o_dir = (float*)calloc(w_height * w_width, sizeof(float));
	int mask_y[9] = { -1,-1,-1,0,0,0,1,1,1 };
	int mask_x[9] = { -1,0,1,-1,0,1,-1,0,1 };

	//compute fx and fy
	for (y = pos_y; y < pos_y + w_height; y++) {
		for (x = pos_x; x < pos_x + w_width; x++) {
			conv_x = 0;
			conv_y = 0;

			//inner product
			for (yy = y - 1; yy <= y + 1; yy++) {
				for (xx = x - 1; xx <= x + 1; xx++) {
					conv_x += input.at<uchar>(yy, xx) * mask_x[(yy - (y - 1)) * 3 + (xx - (x - 1))];
					conv_y += input.at<uchar>(yy, xx) * mask_y[(yy - (y - 1)) * 3 + (xx - (x - 1))];
				}
			}

			conv_x /= 9.0;
			conv_y /= 9.0;

			mag[(y-pos_y) * w_width + (x-pos_x)] = sqrt(conv_x * conv_x + conv_y * conv_y);
			dir = atan2(conv_y, conv_x);
			dir = dir * 180.0 / PI;
			if (dir < 0) dir += 180;
			o_dir[(y - pos_y) * w_width + (x - pos_x)] = dir;
		}
	}
	int idx = 0;
	float normal_pow;
	for (y = 0; y <= w_height - bl_size; y += bl_size / 2) {
		for (x = 0; x <= w_width - bl_size; x += bl_size / 2) {
			for (yy = y; yy < y + bl_size; yy++) {
				for (xx = x; xx < x + bl_size; xx++) {
					deg = o_dir[yy * w_width + xx] / 20.0;
					output[idx * quan + deg] += mag[yy * w_width + xx];
				}
			}
			idx++;
		}
	}
	for (x = 0; x < idx; x++) {
		normal_pow = 0.0;
		for (y = 0; y < quan; y++) {
			normal_pow += (output[x * quan + y] * output[x * quan + y]);
		}
		for (y = 0; y < quan; y++) {
			output[x * quan + y] = output[x * quan + y] / (sqrt(normal_pow));
			if (normal_pow == 0.0)
				output[x * quan + y] = 0.0;
		}
	}
}


float cos_sim(float* ref, float* tar)
{
	float dot = 0.0, size_r = 0.0, size_t = 0.0;
	for (int x = 0; x < 25*quan; x++) {
		dot += ref[x] * tar[x];
		size_r += ref[x] * ref[x];
		size_t += tar[x] * tar[x];
	}
	return dot / (sqrt(size_r) * sqrt(size_t));
}

void compute_sim(Mat input, float * refHOG, int w_height, int w_width)
{
	int height = input.rows;
	int width = input.cols;
	int x, y, xx, yy;
	Mat result(height, width, CV_8UC1);
	float * sim = (float *)calloc(height*width, sizeof(float));
	float max = -1.0;
	float min = 10000000.0;
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			float* tarHOG = (float*)calloc(quan * ((((w_width - bl_size) / (bl_size / 2)) + 1) *
				(((w_height - bl_size) / (bl_size / 2)) + 1)), sizeof(float));
			tar_HOG(input, tarHOG, y, x , w_height, w_width);
			sim[y*width+x] = cos_sim(refHOG, tarHOG);
			if (sim[y*width+x] > max) max = sim[y*width+x];
			if (sim[y*width+x] < min) min = sim[y*width+x];
			free(tarHOG);
		}
	}
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			result.at<uchar>(y, x) = 255 * (sim[y * width + x] - min) / (max - min);
		}
	}
	imwrite("sim.bmp", result);
}

void main()
{
	Mat imgref = imread("face_ref.bmp", CV_LOAD_IMAGE_GRAYSCALE);
	Mat imgtar = imread("face_tar.bmp", CV_LOAD_IMAGE_GRAYSCALE);
	int height = imgref.rows;
	int width = imgref.cols;
	float* refHOG = (float*)calloc(quan * (((width - bl_size) / (bl_size/2))+1) * 
		(((height - bl_size) / (bl_size/2))+1), sizeof(float));
	ref_HOG(imgref, refHOG);
	compute_sim(imgtar, refHOG, height, width);
}