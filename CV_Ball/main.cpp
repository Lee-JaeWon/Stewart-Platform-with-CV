#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <windows.h>
#include <chrono>
#include <thread>

#include "serialcomm.h"

#define MAX 255

using namespace cv;
using namespace std;

void dummy(int, void*) {

}

//Variables(global)
int threshold1 = 30;
Vec3b lower_blue1, upper_blue1, lower_blue2, upper_blue2, lower_blue3, upper_blue3;
Mat img_color;
int target_CAM_X, target_CAM_Y;
Point Target_pt, Center_pt;
int centerX, centerY;
int vector_X, vector_Y;

CSerialComm serialComm;

int Data_Size = 8;
char Data[8] = { 0, };


void mouse_callback(int event, int x, int y, int flags, void* param)
{
	if (event == EVENT_LBUTTONDOWN)
	{
		Vec3b color_pixel = img_color.at<Vec3b>(y, x);

		Mat bgr_color = Mat(1, 1, CV_8UC3, color_pixel);


		Mat hsv_color;
		cvtColor(bgr_color, hsv_color, COLOR_BGR2HSV);

		int hue = hsv_color.at<Vec3b>(0, 0)[0];
		int saturation = hsv_color.at<Vec3b>(0, 0)[1];
		int value = hsv_color.at<Vec3b>(0, 0)[2];

		cout << "hue = " << hue << endl;
		cout << "saturation = " << saturation << endl;
		cout << "value = " << value << endl;


		if (hue < 10)
		{
			cout << "case 1" << endl;
			lower_blue1 = Vec3b(hue - 10 + 180, threshold1, threshold1);
			upper_blue1 = Vec3b(180, 255, 255);
			lower_blue2 = Vec3b(0, threshold1, threshold1);
			upper_blue2 = Vec3b(hue, 255, 255);
			lower_blue3 = Vec3b(hue, threshold1, threshold1);
			upper_blue3 = Vec3b(hue + 10, 255, 255);
		}
		else if (hue > 170)
		{
			cout << "case 2" << endl;
			lower_blue1 = Vec3b(hue, threshold1, threshold1);
			upper_blue1 = Vec3b(180, 255, 255);
			lower_blue2 = Vec3b(0, threshold1, threshold1);
			upper_blue2 = Vec3b(hue + 10 - 180, 255, 255);
			lower_blue3 = Vec3b(hue - 10, threshold1, threshold1);
			upper_blue3 = Vec3b(hue, 255, 255);
		}
		else
		{
			cout << "case 3" << endl;
			lower_blue1 = Vec3b(hue, threshold1, threshold1);
			upper_blue1 = Vec3b(hue + 10, 255, 255);
			lower_blue2 = Vec3b(hue - 10, threshold1, threshold1);
			upper_blue2 = Vec3b(hue, 255, 255);
			lower_blue3 = Vec3b(hue - 10, threshold1, threshold1);
			upper_blue3 = Vec3b(hue, 255, 255);
		}

		cout << "hue = " << hue << endl;
		cout << "#1 = " << lower_blue1 << "~" << upper_blue1 << endl;
		cout << "#2 = " << lower_blue2 << "~" << upper_blue2 << endl;
		cout << "#3 = " << lower_blue3 << "~" << upper_blue3 << endl;
	}
	if (event == EVENT_RBUTTONDOWN) {
		cout << "clicked x : " << x << " // clicked y : " << y << endl;
		target_CAM_X = x;
		target_CAM_Y = y;
	}
}
void Send_data(BYTE data) {

	if (!serialComm.sendCommand(data))
	{
		printf("send command failed\n");
	}

}
void Connect_Uart_Port(const char* _portNum) {
	if (!serialComm.connect(_portNum))
	{
		cout << "connect faliled";
		return;
	}
}
void DisConnect_Uart_Port() {
	serialComm.disconnect();
}
void Transfer_num_to_char(char* txDataBuf, int x_Temp, int y_Temp) {

	
	txDataBuf[0] = '#';
	txDataBuf[1] = (x_Temp % 1000) / 100 + 48;
	txDataBuf[2] = (x_Temp % 100) / 10 + 48;
	txDataBuf[3] = (x_Temp % 10) + 48;
	txDataBuf[4] = ',';
	txDataBuf[5] = (y_Temp % 1000) / 100 + 48;
	txDataBuf[6] = (y_Temp % 100) / 10 + 48;
	txDataBuf[7] = (y_Temp % 10) + 48;
}

//main
int main()
{
	Connect_Uart_Port("COM2");

	namedWindow("img_color");
	setMouseCallback("img_color", mouse_callback);
	//Trackbar
	createTrackbar("threshold", "img_color", &threshold1, MAX, dummy);
	setTrackbarPos("threshold", "img_color", 30);

	Mat img_hsv;
	Scalar red(0, 0, 255);
	Scalar blue(255, 0, 0);
	Scalar yellow(0, 255, 255);

	VideoCapture cap(0, CAP_DSHOW);
	cap.set(CV_CAP_PROP_FRAME_WIDTH, 720);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 720);

	if (!cap.isOpened()) {
		cout << "카메라를 열 수 없습니다." << endl;
		return -1;
	}

	while (1)
	{
		std::chrono::system_clock::time_point StartTime = std::chrono::system_clock::now();

		cap.read(img_color);

		threshold1 = getTrackbarPos("threshold", "img_color");

		cvtColor(img_color, img_hsv, COLOR_BGR2HSV);

		Mat img_mask1, img_mask2, img_mask3, img_mask;
		inRange(img_hsv, lower_blue1, upper_blue1, img_mask1);
		inRange(img_hsv, lower_blue2, upper_blue2, img_mask2);
		inRange(img_hsv, lower_blue3, upper_blue3, img_mask3);
		img_mask = img_mask1 | img_mask2 | img_mask3;


		Mat img_result;
		bitwise_and(img_color, img_color, img_result, img_mask);

		//Labeling
		Mat img_labels, stats, centroids;
		int numOfLables = connectedComponentsWithStats(img_mask, img_labels, stats, centroids, 8, CV_32S);

		// 레이블링 결과에 사각형 그리고, 넘버 표시하기
		for (int j = 1; j < numOfLables; j++) {
			int area = stats.at<int>(j, CC_STAT_AREA);
			int left = stats.at<int>(j, CC_STAT_LEFT);
			int top = stats.at<int>(j, CC_STAT_TOP);
			int width = stats.at<int>(j, CC_STAT_WIDTH);
			int height = stats.at<int>(j, CC_STAT_HEIGHT);
			int radius = height / 2;

			centerX = centroids.at<double>(j, 0);
			centerY = centroids.at<double>(j, 1);

			if (area > 8000) { // 조절 //특정 조건 이상
				circle(img_color, Point(centerX, centerY), radius, red, 5);
				circle(img_color, Point(centerX, centerY), 1, red, 5);


				putText(img_color, "detected", Point(left, top), FONT_HERSHEY_SIMPLEX, 1, blue, 1);

				Center_pt = Point(centerX, centerY);
			}

			vector_X = target_CAM_X - centerX;
			vector_Y = centerY - target_CAM_Y;

			//cout << "vector_X : " << vector_X << " // vector_Y : " << vector_Y << endl;

		}

		//Data Sending
		Transfer_num_to_char(Data, vector_X, vector_Y);
		for (int i = 0; i < Data_Size; i++) {
			Send_data(Data[i]);
			//cout << Data[i] << endl;
		}

		//Target Point
		Target_pt = Point(target_CAM_X, target_CAM_Y);

		//graphics
		circle(img_color, Target_pt, 1, red, 5);
		line(img_color, Target_pt, Center_pt, red, 2, 8, 0);

		//Show
		imshow("img_color", img_color);
		imshow("img_mask", img_mask);
		//imshow("img_result", img_result);

		if (waitKey(1) > 0)
			break;

		std::chrono::system_clock::time_point EndTime = std::chrono::system_clock::now();

		std::chrono::milliseconds mill = std::chrono::duration_cast<std::chrono::milliseconds>(EndTime - StartTime);
		std::this_thread::sleep_for(0.05s - mill);  //제어주기 만드는 것 쓰레드를 sleep해서 지금은 0.05초 50ms
	}

	DisConnect_Uart_Port();

	return 0;
}