#pragma warning(disable:4996)
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <cmath>
#define PI 3.141592

using namespace cv;

void imageResize(Mat input, float scale, int op)
{
	int x, y;
	int height, width;
	int re_height, re_width;
	float pos_x, pos_y;
	int sx, sy;
	float p, q;
	float p1[2], p2[2], p3[2], p4[2];

	height = input.rows;
	width = input.cols;
	re_height = (int)(scale * height);
	re_width = (int)(scale * width);

	Mat result(re_height, re_width, CV_8UC1);

	for (y = 0; y < re_height; y++) {
		for (x = 0; x < re_width; x++) {
			pos_x = (1.0 / scale) * x;
			pos_y = (1.0 / scale) * y;
			//Nearest Neighbor
			if (op == 1) {
				sx = (int)(pos_x + 0.5);
				sy = (int)(pos_y + 0.5);
				result.at<uchar>(y, x) = input.at<uchar>(sy, sx);
			}
			//Average
			else if (op == 2) {
				sx = (int)pos_x;
				sy = (int)pos_y;
				result.at<uchar>(y, x) = 0.25 * (input.at<uchar>(sy, sx) + input.at<uchar>(sy + 1, sx) 
					+ input.at<uchar>(sy + 1, sx + 1) + input.at<uchar>(sy, sx + 1));
			}
			//Bi linear
			else if (op == 3) {
				sx = (int)pos_x;
				sy = (int)pos_y;
				p = pos_x - sx;
				q = pos_y - sy;
				result.at<uchar>(y, x) = (1 - p) * (1 - q) * (input.at<uchar>(sy, sx)) 
					+ p * (1 - q) * (input.at<uchar>(sy, sx + 1))
					+ (1 - p) * q * (input.at<uchar>(sy + 1, sx)) 
					+ p * q * (input.at<uchar>(sy + 1, sx + 1));
			}
			else {}
		}
	}
	if (op == 1) {
		imwrite("nn.jpg", result);
	}
	else if (op == 2) {
		imwrite("average.jpg", result);
	}
	else {
		imwrite("bi_lin.jpg", result);
	}
}

void imageRotate(Mat input, float degree, int op)
{
	int x, y;
	int height, width;
	float rad = degree * PI / 180.0;
	float R[2][2] = { {cos(rad), sin(rad)}, {-sin(rad), cos(rad)} };
	float sx, sy;
	int pos_x, pos_y;
	float p, q;

	height = input.rows;
	width = input.cols;

	Mat result(height, width, CV_8UC1);

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			sx = R[0][0] * (x-width/2) + R[0][1] * (y-height/2);
			sy = R[1][0] * (x-width/2) + R[1][1] * (y-height/2);
			sx = sx + width / 2;
			sy = sy + height / 2;
			if (op == 1) {
				pos_x = (int)(sx + 0.5);
				pos_y = (int)(sy + 0.5);
				if (pos_x < 0 || pos_x > width - 1 || pos_y < 0 || pos_y > height - 1) {}
				else {
					result.at<uchar>(y, x) = input.at<uchar>(pos_y, pos_x);
				}
			}
			else if (op == 2) {
				pos_x = (int)sx;
				pos_y = (int)sy;
				p = sx-pos_x;
				q = sy-pos_y;
				if (pos_x < 0 || pos_x > width - 1 || pos_y < 0 || pos_y > height - 1) {}
				else {
					result.at<uchar>(y, x) = (1 - p) * (1 - q) * (input.at<uchar>(pos_y, pos_x)) 
						+ p * (1 - q) * (input.at<uchar>(pos_y, pos_x + 1))
						+ (1 - p) * q * (input.at<uchar>(pos_y + 1, pos_x)) 
						+ p * q * (input.at<uchar>(pos_y + 1, pos_x + 1));
				}

			}
			else {}
		}
	}
	if (op == 1) {
		imwrite("nn-rotate.jpg", result);
	}
	else {
		imwrite("bi-rotate.jpg", result);
	}
}

void main()
{
	float scale, degree;
	int option;
	Mat imgColor = imread("test2.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	printf("Input your degree : ");
	scanf("%f", &degree);
	printf("1 : NN, 2 : Average, 3 : Bi-Linear\n");
	printf("1 : NN, 2 : Bi-Linear\n");
	printf("Input your option : ");
	scanf("%d", &option);

	imageResize(imgColor, scale, option);
	imageRotate(imgColor, degree, option);
}
