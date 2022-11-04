/*
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;

void harris_corner(Mat input, Mat output)
{
	int x, y, xx, yy;
	int height = input.rows;
	int width = input.cols;
	Scalar c;
	Point pCenter;
	int radius = 3;
	float conv_x, conv_y;
	float ixix, iyiy, ixiy, det, tr;
	float k = 0.04, th = 1000000;
	float min = 100000, max = -100000;
	float* ix = (float*)calloc(height * width, sizeof(float));
	float* iy = (float*)calloc(height * width, sizeof(float));
	float* R = (float*)calloc(height * width, sizeof(float));
	int* cornerMap = (int*)calloc(height * width, sizeof(float));
	int b_size = 3;
	int win_size = 3;
	const int size = 9;

	int mask_x[size] = { -1,-1,-1,0,0,0,1,1,1 };
	int mask_y[size] = { -1,0,1,-1,0,1,-1,0,1 };
	//compute fx and fy
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			conv_x = 0;
			conv_y = 0;
			//inner product
			for (yy = y - b_size / 2; yy <= y + b_size / 2; yy++) {
				for (xx = x - b_size / 2; xx <= x + b_size / 2; xx++) {
					if (yy >= 0 && yy < height && xx >= 0 && xx < width) {
						conv_x += input.at<uchar>(yy, xx) * mask_x[(yy - (y - 1)) * b_size + (xx - (x - 1))];
						conv_y += input.at<uchar>(yy, xx) * mask_y[(yy - (y - 1)) * b_size + (xx - (x - 1))];
					}
				}
			}
			conv_x /= 9;
			conv_y /= 9;
			ix[y * width + x] = conv_x;
			iy[y * width + x] = conv_y;

			output.at<Vec3b>(y, x)[2] = input.at<uchar>(y, x);
			output.at<Vec3b>(y, x)[1] = input.at<uchar>(y, x);
			output.at<Vec3b>(y, x)[0] = input.at<uchar>(y, x);
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
			if (R[y * width + x] > th) cornerMap[y * width + x] = 1;
			if (R[y * width + x] > max) max = R[y * width + x];
			if (R[y * width + x] < min) min = R[y * width + x];
		}
	}

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			if (cornerMap[y * width + x] == 1) {
				pCenter.x = x;
				pCenter.y = y;
				c.val[0] = 0;
				c.val[1] = 255;
				c.val[2] = 0;
				circle(output, pCenter, radius, c, 2, 8, 0);
			}
		}
	}
	free(R);
	free(ix);
	free(iy);
	free(cornerMap);
}


void main()
{
	VideoCapture capture(0);
	Mat frame;
	Mat frameGray;

	if (!capture.isOpened()) {
		printf("Couldn't open the web camera...\n");
		return;
	}

	while (true) {
		capture >> frame;
		//frame이 color입력이기 때문에 gray로 변경해주어야 함
		cvtColor(frame, frameGray, COLOR_BGR2GRAY);
		//cornerMap은 frame이 while문안에서 frame이 생성되므로 while문 안에 위치
		Mat cornerMap(frameGray.rows, frameGray.cols, CV_8UC3);
		harris_corner(frameGray, cornerMap);
		imshow("Video", cornerMap);
		if (waitKey(30) >= 0) break;
	}
}
*/

