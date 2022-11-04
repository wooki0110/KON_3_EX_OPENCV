#include <cmath>
#define PI 3.141592
#include "utils.h"

using namespace cv;

Mat imageResize(Mat input, float scale)
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
			//Bi linear
			sx = (int)pos_x;
			sy = (int)pos_y;
			p = pos_x - sx;
			q = pos_y - sy;
			result.at<uchar>(y, x) = (1 - p) * (1 - q) * (input.at<uchar>(sy, sx))
				+ p * (1 - q) * (input.at<uchar>(sy, sx + 1))
				+ (1 - p) * q * (input.at<uchar>(sy + 1, sx))
				+ p * q * (input.at<uchar>(sy + 1, sx + 1));
		}
	}
	return result;
}