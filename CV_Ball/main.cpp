#include "CV_Ball.h"

#include<iostream>
#include<opencv2\core.hpp>
#include<opencv\cv.h>
#include<opencv2\highgui.hpp>
#include<opencv2\imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <windows.h>

using namespace cv;
using namespace std;

#define MAX 255

int main(int, char**)
{
	//VideoCapture cap(0);
	VideoCapture cap(0, CAP_DSHOW);
	cap.set(CV_CAP_PROP_FRAME_WIDTH, 720);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 720);
	cap.set(CAP_PROP_FPS, 30);

	//
	cv::Mat img;
	int thredshold;

	//Trackbar
	namedWindow("Image_res");
	createTrackbar("thredshold", "Image_res", &thredshold, MAX);
	setTrackbarPos("thredshold", "Image_res", 100);

	while (1)
	{
		thredshold = getTrackbarPos("thredshold", "Image_res");

		cap >> img;

		//RGB to HSV
		Mat img_hsv;
		cvtColor(img, img_hsv, COLOR_BGR2HSV);

		//Thresholding
		Mat img_threshold;
		threshold(img, img_threshold, MAX - thredshold, MAX, THRESH_BINARY);

		//Erode, Dilate
		Mat img_dilate;
		Mat img_erode;
		dilate(img_threshold, img_dilate, Mat::ones(Size(3, 3), CV_8UC1), Point(-1, -1));
		erode(img_dilate, img_erode, Mat::ones(Size(3, 3), CV_8UC1), Point(-1, -1));

		//to Bin
		Mat img_gray;
		cvtColor(img_erode, img_gray, COLOR_BGR2GRAY);

		//Labeling
		Mat img_labels, stats, centroids;
		int numOfLables = connectedComponentsWithStats(img_gray, img_labels, stats, centroids, 8, CV_32S);

		// 레이블링 결과에 사각형 그리고, 넘버 표시하기
		for (int j = 1; j < numOfLables; j++) {
			int area = stats.at<int>(j, CC_STAT_AREA);
			int left = stats.at<int>(j, CC_STAT_LEFT);
			int top = stats.at<int>(j, CC_STAT_TOP);
			int width = stats.at<int>(j, CC_STAT_WIDTH);
			int height = stats.at<int>(j, CC_STAT_HEIGHT);

			rectangle(img, Point(left, top), Point(left + width, top + height),
				Scalar(0, 0, 255), 1);

			putText(img, to_string(j), Point(left + 20, top + 20), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 1);
		}

		//Display
		imshow("Image_res", img);
		imshow("img_original", img);
		imshow("img_hsv", img_hsv);
		imshow("img_dilate", img_dilate);
		imshow("img_erode", img_erode);

		cout << "numOfLables : " << numOfLables - 1 << endl;	// 최종 넘버링에서 1을 빼줘야 함

		if (waitKey(10) == 27)
			return 0;
	}

	return 0;
}