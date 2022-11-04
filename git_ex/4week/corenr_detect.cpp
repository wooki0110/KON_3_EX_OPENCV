/*
#pragma warning(disable:4996)
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#define PI 3.141592
#define bl_size 16

using namespace cv;

void harris_corner(Mat input_r, Mat input_t)
{
	int x, y, xx, yy, z;
	int height = input_r.rows;
	int width = input_r.cols;
	Scalar c;
	Point pCenter;
	int radius = 3;
	float conv_x, conv_y;
	float dir;
	float ixix, iyiy, ixiy, det, tr;
	float k = 0.04, th = 1000000000;
	float min = 1000000, max = -100000;
	float* ix = (float*)calloc(height * width, sizeof(float));
	float* iy = (float*)calloc(height * width, sizeof(float));
	float* R = (float*)calloc(height * width, sizeof(float));
	float* mag = (float*)calloc(height * width, sizeof(float));
	float* o_dir = (float*)calloc(height * width, sizeof(float));
	int* cornerMap = (int*)calloc(height * width, sizeof(float));
	int* cornerMap_t = (int*)calloc(height * width, sizeof(float));
	Mat output(height, width*2, CV_8UC3);
	int b_size = 3;
	int win_size = 3;
	const int size = 9;
	float normal_pow;
	int count_r = 0, count_t = 0, max_sm = 0, max_idx = 0;
	int deg, idx_r = 0, idx_t = 0;
	float** dir_r_mag, ** dir_t_mag;
	int* pos_r, * pos_t, *idx_sm;
	int sm_count = 0;
	FILE* fp;
	
	int mask_y[size] = { -1,-1,-1,0,0,0,1,1,1 };
	int mask_x[size] = { -1,0,1,-1,0,1,-1,0,1 };
	//compute fx and fy for ref
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			conv_x = 0;
			conv_y = 0;
			//inner product
			for (yy = y - b_size / 2; yy <= y + b_size / 2; yy++) {
				for (xx = x - b_size / 2; xx <= x + b_size / 2; xx++) {
					if (yy >= 0 && yy < height && xx >= 0 && xx < width) {
						conv_x += input_r.at<uchar>(yy, xx) * mask_x[(yy - (y - 1)) * b_size + (xx - (x - 1))];
						conv_y += input_r.at<uchar>(yy, xx) * mask_y[(yy - (y - 1)) * b_size + (xx - (x - 1))];
					}
				}
			}
			mag[y * width + x] = sqrt(conv_x * conv_x + conv_y * conv_y);
			dir = atan2(conv_y, conv_x);
			dir = dir * 180.0 / PI;
			o_dir[y * width + x] = dir;
			ix[y * width + x] = conv_x;
			iy[y * width + x] = conv_y;
			output.at<Vec3b>(y, x)[0] = input_r.at<uchar>(y, x);
			output.at<Vec3b>(y, x)[1] = input_r.at<uchar>(y, x);
			output.at<Vec3b>(y, x)[2] = input_r.at<uchar>(y, x);
		}
	}
	//R computation
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			ixix = 0;
			iyiy = 0;
			ixiy = 0;
			for (yy = y - win_size / 2; yy <= y + win_size / 2; yy++) {
				for (xx = x - win_size / 2; xx <= x + win_size / 2; xx++) {
					if (yy >= 0 && yy < height && xx >= 0 && xx < width) {
						ixix += ix[yy * width + xx] * ix[yy * width + xx];
						ixiy += ix[yy * width + xx] * iy[yy * width + xx];
						iyiy += iy[yy * width + xx] * iy[yy * width + xx];
					}
				}
			}
			det = ixix * iyiy - ixiy * ixiy;
			tr = ixix + iyiy;
			R[y * width + x] = det - k * tr * tr;

			//draw circle at coner
			if (R[y * width + x] > th) {
				cornerMap[y * width + x] = 1;
				count_r++;
			}
			if (R[y * width + x] > max) max = R[y * width + x];
			if (R[y * width + x] < min) min = R[y * width + x];
		}
	}
	dir_r_mag = (float**)calloc(count_r, sizeof(float*));
	for (x = 0; x < count_r; x++) {
		dir_r_mag[x] = (float*)calloc(9, sizeof(float));
	}
	pos_r = (int*)calloc(count_r * 2, sizeof(float));
	fp = fopen("HOG.csv", "w");
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			if (cornerMap[y * width + x] == 1) {
				for (yy = y - bl_size / 2; yy <= y + bl_size / 2; yy++) {
					for (xx = x - bl_size / 2; xx <= x + bl_size / 2; xx++) {
						if (yy >= 0 && yy < height && xx >= 0 && xx < width) {
							deg = o_dir[yy * width + xx] / 20.0;
							dir_r_mag[idx_r][deg] += mag[yy * width + xx];
						}
					}
				}
				normal_pow = 0.0;
				for (z = 0; z < 9; z++) {
					normal_pow += (dir_r_mag[idx_r][z] * dir_r_mag[idx_r][z]);
				}
				for (z = 0; z < 9; z++) {
					dir_r_mag[idx_r][z] = dir_r_mag[idx_r][z] / (sqrt(normal_pow) + 0.0000000000001);
					fprintf(fp, "%d%d, %f\n", idx_r, z, dir_r_mag[idx_r][z]);
				}
				pos_r[2 * idx_r] = x;
				pos_r[2 * idx_r+1] = y;
				pCenter.x = x;
				pCenter.y = y;
				c.val[0] = 0;
				c.val[1] = 255;
				c.val[2] = 0;
				circle(output, pCenter, radius, c, 2, 8, 0);
				idx_r++;
			}
		}
	}
	//compute fx and fy for bmp
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			conv_x = 0;
			conv_y = 0;
			//inner product
			for (yy = y - b_size / 2; yy <= y + b_size / 2; yy++) {
				for (xx = x - b_size / 2; xx <= x + b_size / 2; xx++) {
					if (yy >= 0 && yy < height && xx >= 0 && xx < width) {
						conv_x += input_t.at<uchar>(yy, xx) * mask_x[(yy - (y - 1)) * b_size + (xx - (x - 1))];
						conv_y += input_t.at<uchar>(yy, xx) * mask_y[(yy - (y - 1)) * b_size + (xx - (x - 1))];
					}
				}
			}
			mag[y * width + x] = sqrt(conv_x * conv_x + conv_y * conv_y);
			dir = atan2(conv_y, conv_x);
			dir = dir * 180.0 / PI;
			o_dir[y * width + x] = dir;
			ix[y * width + x] = conv_x;
			iy[y * width + x] = conv_y;
			output.at<Vec3b>(y, x+width)[0] = input_t.at<uchar>(y, x);
			output.at<Vec3b>(y, x+width)[1] = input_t.at<uchar>(y, x);
			output.at<Vec3b>(y, x+width)[2] = input_t.at<uchar>(y, x);
		}
	}
	//R computation
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			ixix = 0;
			iyiy = 0;
			ixiy = 0;
			for (yy = y - win_size / 2; yy <= y + win_size / 2; yy++) {
				for (xx = x - win_size / 2; xx <= x + win_size / 2; xx++) {
					if (yy >= 0 && yy < height && xx >= 0 && xx < width) {
						ixix += ix[yy * width + xx] * ix[yy * width + xx];
						ixiy += ix[yy * width + xx] * iy[yy * width + xx];
						iyiy += iy[yy * width + xx] * iy[yy * width + xx];
					}
				}
			}
			det = ixix * iyiy - ixiy * ixiy;
			tr = ixix + iyiy;
			R[y * width + x] = det - k * tr * tr;

			//draw circle at coner
			if (R[y * width + x] > th) {
				cornerMap_t[y * width + x] = 1;
				count_t++;
			}
			if (R[y * width + x] > max) max = R[y * width + x];
			if (R[y * width + x] < min) min = R[y * width + x];
		}
	}
	free(R);
	free(ix);
	free(iy);
	dir_t_mag = (float**)calloc(count_t, sizeof(float*));
	for (x = 0; x < count_t; x++) {
		dir_t_mag[x] = (float*)calloc(9, sizeof(float));
	}
	pos_t = (int*)calloc(count_t * 2, sizeof(int));
	fp = fopen("HOG1.csv", "w");
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			if (cornerMap_t[y * width + x] == 1) {
				for (yy = y - bl_size / 2; yy <= y + bl_size / 2; yy++) {
					for (xx = x - bl_size / 2; xx <= x + bl_size / 2; xx++) {
						if (yy >= 0 && yy < height && xx >= 0 && xx < width) {
							deg = o_dir[yy * width + xx] / 20.0;
							dir_t_mag[idx_t][deg] += mag[yy * width + xx];
						}
					}
				}
				normal_pow = 0.0;
				for (z = 0; z < 9; z++) {
					normal_pow += (dir_t_mag[idx_t][z] * dir_t_mag[idx_t][z]);
				}
				for (z = 0; z < 9; z++) {
					dir_t_mag[idx_t][z] = dir_t_mag[idx_t][z] / (sqrt(normal_pow) + 0.0000000000001);
					fprintf(fp, "%d%d, %f\n", idx_t, z, dir_t_mag[idx_t][z]);
				}
				pos_t[2 * idx_t] = x + width;
				pos_t[2 * idx_t + 1] = y;
				pCenter.x = x + width;
				pCenter.y = y;
				c.val[0] = 0;
				c.val[1] = 255;
				c.val[2] = 0;
				circle(output, pCenter, radius, c, 2, 8, 0);
				idx_t++;
			}
		}
	}
	imwrite("corner.bmp", output);
	free(o_dir);
	free(mag);
	fclose(fp);
	idx_sm = (int*)calloc(idx_r, sizeof(int));
	printf("%d %d\n", idx_r, idx_t);
	for (y = 0; y < idx_t; y++) {
		max_sm = 0;
		max_idx = 0;
		for (x = 0; x < idx_r; x++) {
			sm_count = 0;
			for (xx = 0; xx < 9; xx++) {
				if (abs(dir_r_mag[x][xx] - dir_t_mag[y][xx]) < 0.006) {
					sm_count++;
				}
			}
			idx_sm[x] = sm_count;
			if (max_sm < sm_count) {
				max_sm = sm_count;
				max_idx = x;
			}
		}
		printf("%d  %d  %d\n", max_sm, y, max_idx);
		line(output, Point(pos_r[2 * max_idx], pos_r[2 * max_idx + 1]), Point(pos_t[2 * y], pos_t[2 * y + 1])
				, Scalar(255, 0, 0), 2, 9, 0);
	}
	imwrite("result.bmp", output);
	free(dir_r_mag);
	free(dir_t_mag);
	free(idx_sm);
}

void main()
{
	Mat imgref = imread("ref.bmp", CV_LOAD_IMAGE_GRAYSCALE);
	Mat imgtar = imread("tar.bmp", CV_LOAD_IMAGE_GRAYSCALE);
	harris_corner(imgref, imgtar);
}
*/