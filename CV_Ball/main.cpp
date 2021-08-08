/*===================================================================================
 * Ball Detecting with opencv
 * using PID control and send to MCU
 * main.c
 *
 * Author : 이재원
===================================================================================*/

////////////////////////////   Includes   ////////////////////////////////////
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <windows.h>
#include <chrono>
#include <thread>
#include "serialcomm.h"
#include "PID_Control.h"
#include "Matrix_calc.h"
//////////////////////////////////////////////////////////////////////////////

//Value define
#define MAX 255
#define PI 3.141592

///////////////////////////   namespace   ////////////////////////////////////
using namespace cv;
using namespace std;
using namespace Eigen;
//////////////////////////////////////////////////////////////////////////////

//etc
void dummy(int, void*) {

}
string intro = "\n============= Ball Balancing with Stewart Platform. =============\n * Author : 이재원\n * Visual Studio 2019\n";

///////////////////////////   Variables(global)   ///////////////////////////
//CV
int threshold1 = 50;
Vec3b lower_blue1, upper_blue1, lower_blue2, upper_blue2, lower_blue3, upper_blue3;
Mat img_change;
Mat img_color;
Mat dst; // 투시 변환 결과
// Color variables
Scalar red(0, 0, 255);
Scalar blue(255, 0, 0);
Scalar yellow(0, 255, 255);
//Point Variables
double target_CAM_X, target_CAM_Y;
Point Target_pt, Center_pt;
double centerX, centerY;
double vector_X, vector_Y;
//투시 변환
static int cnt = 0;
Point2f srcQuad[4], dstQuad[4];
int w = 600, h = 600;
//Serial communication variables
CSerialComm serialComm;
int Data_Size = 10;
char Data[10] = { 0, };
//Hardware
static int HardWare_height = 120; //mm
//////////////////////////////////////////////////////////////////////////////

void mouse_callback(int event, int x, int y, int flags, void* param)
{
	if (event == EVENT_LBUTTONDOWN) {
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


		if (hue < 10) {
			cout << "case 1" << endl;
			lower_blue1 = Vec3b(hue - 10 + 180, threshold1, threshold1);
			upper_blue1 = Vec3b(180, 255, 255);
			lower_blue2 = Vec3b(0, threshold1, threshold1);
			upper_blue2 = Vec3b(hue, 255, 255);
			lower_blue3 = Vec3b(hue, threshold1, threshold1);
			upper_blue3 = Vec3b(hue + 10, 255, 255);
		}
		else if (hue > 170) {
			cout << "case 2" << endl;
			lower_blue1 = Vec3b(hue, threshold1, threshold1);
			upper_blue1 = Vec3b(180, 255, 255);
			lower_blue2 = Vec3b(0, threshold1, threshold1);
			upper_blue2 = Vec3b(hue + 10 - 180, 255, 255);
			lower_blue3 = Vec3b(hue - 10, threshold1, threshold1);
			upper_blue3 = Vec3b(hue, 255, 255);
		}
		else {
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

	//if (event == EVENT_MBUTTONDOWN) {
	//	cout << "투시 변환 영역 선택 중..." << endl;
	//	if (cnt < 4) {
	//		srcQuad[cnt++] = Point2f(x, y); // 클릭한 좌표를 src배열에 저장

	//		circle(img_change, Point(x, y), 1, red, 5); //빨간색 원

	//		if (cnt == 4) {
	//			dstQuad[0] = Point2f(0, 0); // 결과 영상 좌표
	//			dstQuad[1] = Point2f(w - 1, 0);
	//			dstQuad[2] = Point2f(w - 1, h - 1);
	//			dstQuad[3] = Point2f(0, h - 1);
	//		}
	//	}
	//}
}
void Send_data(BYTE data) {
	if (!serialComm.sendCommand(data)) {
		//printf("send command failed\n");
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
void Transfer_num_to_char(char* txDataBuf, int lenght_1, int lenght_2, int lenght_3) {

	txDataBuf[0] = 's';
	txDataBuf[1] = (lenght_1 % 100) / 10 + 48;
	txDataBuf[2] = (lenght_1 % 10) + 48;
	txDataBuf[3] = ',';
	txDataBuf[4] = (lenght_2 % 100) / 10 + 48;
	txDataBuf[5] = (lenght_2 % 10) + 48;
	txDataBuf[6] = '/';
	txDataBuf[7] = (lenght_3 % 100) / 10 + 48;
	txDataBuf[8] = (lenght_3 % 10) + 48;
	txDataBuf[9] = 'e';

}

//main
int main()
{
	Connect_Uart_Port("COM2");

	////////////////////   TrackBar, Window   ////////////////////////////
	namedWindow("img_color");
	setMouseCallback("img_color", mouse_callback);
	createTrackbar("threshold", "img_color", &threshold1, MAX, dummy);
	setTrackbarPos("threshold", "img_color", 30);
	//////////////////////////////////////////////////////////////////////
	Mat img_hsv;
	Mat img_mask1, img_mask2, img_mask3, img_mask;
	Mat img_result;
	Mat img_labels, stats, centroids;
	///////////////////   class   //////////////////////
	//PID
	PID_Control PID_SET;
	//Matrix
	Matrix_ball mxb;
	////////////////////////////////////////////////////

	//Camera Set
	VideoCapture cap(1, CAP_DSHOW);
	cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

	if (!cap.isOpened()) {
		cout << "카메라를 열 수 없습니다." << endl;
		return -1;
	}
	else cout << intro << endl;

	while (1) {

		//std::chrono::system_clock::time_point StartTime = std::chrono::system_clock::now();

		cap.read(img_color);

		threshold1 = getTrackbarPos("threshold", "img_color");

		cvtColor(img_color, img_hsv, COLOR_BGR2HSV);

		inRange(img_hsv, lower_blue1, upper_blue1, img_mask1);
		inRange(img_hsv, lower_blue2, upper_blue2, img_mask2);
		inRange(img_hsv, lower_blue3, upper_blue3, img_mask3);
		img_mask = img_mask1 | img_mask2 | img_mask3;

		bitwise_and(img_color, img_color, img_result, img_mask);

		//Labeling
		int numOfLables = connectedComponentsWithStats(img_mask, img_labels, stats, centroids, 8, CV_32S);

		//레이블링 결과
		for (int j = 1; j < numOfLables; j++) {
			int area = stats.at<int>(j, CC_STAT_AREA);
			int left = stats.at<int>(j, CC_STAT_LEFT);
			int top = stats.at<int>(j, CC_STAT_TOP);
			int width = stats.at<int>(j, CC_STAT_WIDTH);
			int height = stats.at<int>(j, CC_STAT_HEIGHT);
			int radius = height / 2;

			if (area > 4000) { // 조절 //특정 조건 이상

				centerX = centroids.at<double>(j, 0);
				centerY = centroids.at<double>(j, 1);

				circle(img_color, Point(centerX, centerY), radius, red, 5);
				circle(img_color, Point(centerX, centerY), 1, red, 5);

				putText(img_color, "Detecting", Point(left, top), FONT_HERSHEY_SIMPLEX, 1, blue, 1);

				Center_pt = Point(centerX, centerY);
			}
			vector_X = (target_CAM_X - centerX) * 0.46875;
			vector_Y = ((-1) * (target_CAM_Y - centerY)) * 0.625;

		}
		////PID Control
		//long length = sqrt(vector_X * vector_X + vector_Y * vector_Y);
		//PID_SET.PID_Control_long(0, length);
		//vector_X = sqrt((length * length) - (vector_Y * vector_Y));
		//vector_Y = sqrt((length * length) - (vector_X * vector_X));

		//Matrix
		mxb.find_theta(vector_X, HardWare_height);
		mxb.find_phi(vector_Y, HardWare_height);
		mxb.BR_p_Set();
		mxb.P_vector_Set();
		mxb.B_vector_Set();
		mxb.T_vector_Set(vector_X, vector_Y, HardWare_height);

		mxb.L1_vector = mxb.T_vector.transpose() + (mxb.BR_p * mxb.P1_vector.transpose()) - mxb.B1_vector.transpose();
		mxb.L2_vector = mxb.T_vector.transpose() + (mxb.BR_p * mxb.P2_vector.transpose()) - mxb.B2_vector.transpose();
		mxb.L3_vector = mxb.T_vector.transpose() + (mxb.BR_p * mxb.P3_vector.transpose()) - mxb.B3_vector.transpose();

		mxb.L1_Size = mxb.vector_size(mxb.L1_vector(0, 0), mxb.L1_vector(1, 0), mxb.L1_vector(2, 0));
		mxb.L2_Size = mxb.vector_size(mxb.L2_vector(0, 0), mxb.L2_vector(1, 0), mxb.L2_vector(2, 0));
		mxb.L3_Size = mxb.vector_size(mxb.L3_vector(0, 0), mxb.L3_vector(1, 0), mxb.L3_vector(2, 0));

		cout << (int)mxb.L1_Size / 10 << " // " << (int)mxb.L2_Size / 10 << " // " << (int)mxb.L3_Size / 10 << endl;

		//Data Sending
		Transfer_num_to_char(Data, (int)mxb.L1_Size / 10, (int)mxb.L2_Size / 10, (int)mxb.L3_Size / 10);
		for (int i = 0; i < Data_Size; i++) {
			Send_data(Data[i]);
		}

		//Target Point
		Target_pt = Point(target_CAM_X, target_CAM_Y);

		//graphics
		circle(img_color, Target_pt, 1, red, 5);
		line(img_color, Target_pt, Center_pt, red, 2, 8, 0);

		//Show
		imshow("img_color", img_color);

		if (waitKey(1) > 0)
			break;

		//std::chrono::system_clock::time_point EndTime = std::chrono::system_clock::now();
		//std::chrono::milliseconds mill = std::chrono::duration_cast<std::chrono::milliseconds>(EndTime - StartTime);
		//std::this_thread::sleep_for(0.05s - mill);  //제어주기 만드는 것 쓰레드를 sleep해서 지금은 0.05초 50ms
	}

	DisConnect_Uart_Port();

	return 0;
}