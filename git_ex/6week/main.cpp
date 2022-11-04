#define _CRT_SECURE_NO_WARNINGS
#include <opencv2/features2d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "utils.h"

#include <cmath>
#define PI 3.141592

using namespace cv;
using namespace std;

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

//ORB settings
int ORB_MAX_KPTS = 400;
float ORB_SCALE_FACTOR = 1.2;
int ORB_PYRAMID_LEVELS = 4;
float ORB_EDGE_THRESHOLD = 31.0;
int ORB_FIRST_PYRAMID_LEVEL = 0;
int ORB_WTA_K = 2;
int ORB_PATCH_SIZE = 31;
//Some image matching options
float MIN_H_ERROR = 2.50f; //Maximum error in pixels to accept an inlier
float DRATIO = 0.80f;

void main() {
	Mat img1, img1_32, img2, img2_32;
	string img_path1, img_path2, homography_path;
	double t1 = 0.0, t2 = 0.0;
	vector<KeyPoint> kpts1_orb;
	Mat desc1_orb;
	float ratio_orb = 0.0;
	img1 = imread("ref.jpg", 0);
	img1 = imageResize(img1, 0.75);

	VideoCapture capture(0);
	Mat frame;
	Mat frameGray;
	//Convert the images to float
	img1.convertTo(img1_32, CV_32F, 1.0 / 255.0, 0);
	

	//Color images for results visualization
	Mat img1_rgb_orb = Mat(Size(img1.cols, img1.rows), CV_8UC3);
	
	//ORBFeatures
	Ptr<ORB>orb = ORB::create(ORB_MAX_KPTS, ORB_SCALE_FACTOR, ORB_PYRAMID_LEVELS, ORB_EDGE_THRESHOLD, ORB_FIRST_PYRAMID_LEVEL, ORB_WTA_K, ORB::HARRIS_SCORE,
		ORB_PATCH_SIZE);
	t1 = getTickCount();
	orb->detectAndCompute(img1, noArray(), kpts1_orb, desc1_orb, false);
	while (true)
	{
		capture >> frame;
		cvtColor(frame, img2, COLOR_BGR2GRAY);
		img2 = imageResize(img2, 0.75);

		frameGray.convertTo(img2_32, CV_32F, 1.0 / 255.0, 0);

		vector<KeyPoint> kpts2_orb;
		vector<Point2f> matches_orb, inliers_orb;
		vector<vector<DMatch>> dmatches_orb;
		Mat desc2_orb;
		int nmatches_orb = 0, ninliers_orb = 0, noutliers_orb = 0;
		int nkpts1_orb = 0, nkpts2_orb = 0;
		double torb = 0.0;	//Create the L2 and L1 matchers
		Ptr<DescriptorMatcher> matcher_l2 = DescriptorMatcher::create("BruteForce");
		Ptr<DescriptorMatcher> matcher_l1 = DescriptorMatcher::create("BruteForce-Hamming");

		Mat img2_rgb_orb = Mat(Size(img2.cols, img1.rows), CV_8UC3);
		Mat img_com_orb = Mat(Size(img1.cols * 2, img1.rows), CV_8UC3);
		
		orb->detectAndCompute(img2, noArray(), kpts2_orb, desc2_orb, false);

		matcher_l1->knnMatch(desc1_orb, desc2_orb, dmatches_orb, 2);
		matches2points_nndr(kpts1_orb, kpts2_orb, dmatches_orb, matches_orb, DRATIO);
		compute_inliers_ransac(matches_orb, inliers_orb, MIN_H_ERROR, false);

		nkpts1_orb = kpts1_orb.size();
		nkpts2_orb = kpts2_orb.size();
		nmatches_orb = matches_orb.size() / 2;
		ninliers_orb = inliers_orb.size() / 2;
		noutliers_orb = nmatches_orb - ninliers_orb;
		ratio_orb = 100.0 * (float)(ninliers_orb) / (float)(nmatches_orb);
		t2 = cv::getTickCount();

		torb = 1000.0 * (t2 - t1) / getTickFrequency();
		cvtColor(img1, img1_rgb_orb, cv::COLOR_GRAY2BGR);
		cvtColor(img2, img2_rgb_orb, cv::COLOR_GRAY2BGR);
		draw_keypoints(img1_rgb_orb, kpts1_orb);
		draw_keypoints(img2_rgb_orb, kpts2_orb);
		draw_inliers(img1_rgb_orb, img2_rgb_orb, img_com_orb, inliers_orb, 0);
		imshow("ORB", img_com_orb);
		if(waitKey(30)>=0) break;
	}
}
