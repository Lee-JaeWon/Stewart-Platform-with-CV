#include "Matrix_calc.h"

Matrix_ball::Matrix_ball() {
	//theta, phi
	this->theta = 0;
	this->phi = 0;
	//process
	this->BR_p = MatrixXd(3, 3);
	this->T_vector = MatrixXd(1, 3);
	this->P1_vector = MatrixXd(1, 3);
	this->P2_vector = MatrixXd(1, 3);
	this->P3_vector = MatrixXd(1, 3);
	this->B1_vector = MatrixXd(1, 3);
	this->B2_vector = MatrixXd(1, 3);
	this->B3_vector = MatrixXd(1, 3);
	//Result
	this->L1_vector = MatrixXd(1, 3);
	this->L2_vector = MatrixXd(1, 3);
	this->L3_vector = MatrixXd(1, 3);
}
void Matrix_ball::find_theta(double vector_X, const int Hard_height) {
	this->theta = atan(vector_X / Hard_height) * 57.2957;
}
void Matrix_ball::find_phi(double vector_Y, const int Hard_height) {
	this->phi = atan(vector_Y / Hard_height) * 57.2957;
}
void Matrix_ball::BR_p_Set() {
	this->BR_p(0, 0) = cos(this->theta) * 57.2957;
	this->BR_p(0, 1) = (sin(this->theta) * sin(this->phi)) * 57.2957;
	this->BR_p(0, 2) = (cos(this->phi) * sin(this->phi)) * 57.2957;
	this->BR_p(1, 0) = 0;
	this->BR_p(1, 1) = cos(this->phi) * 57.2957;
	this->BR_p(1, 2) = (-1) * (sin(this->phi)) * 57.2957;
	this->BR_p(2, 0) = (-1) * (sin(this->theta)) * 57.2957;
	this->BR_p(2, 1) = (cos(this->theta) * sin(this->phi)) * 57.2957;
	this->BR_p(2, 2) = (cos(this->phi) * cos(this->theta)) * 57.2957;
}
void Matrix_ball::T_vector_Set(double vector_X, double vector_Y, const int Hard_height) {
	this->T_vector(0, 0) = vector_X; //x
	this->T_vector(0, 1) = vector_Y; //y
	this->T_vector(0, 2) = Hard_height; //z
}
void Matrix_ball::P_vector_Set() {
	this->P1_vector(0, 0) = 212.132;
	this->P1_vector(0, 1) = 15;
	this->P1_vector(0, 2) = 0;
	this->P2_vector(0, 0) = -212.132;
	this->P2_vector(0, 1) = 150;
	this->P2_vector(0, 2) = 0;
	this->P3_vector(0, 0) = 0;
	this->P3_vector(0, 1) = -150;
	this->P3_vector(0, 2) = 0;
}
void Matrix_ball::B_vector_Set() { //하드웨어 스펙으로 체인지
	this->B1_vector(0, 0) = 212.132;
	this->B1_vector(0, 1) = 150;
	this->B1_vector(0, 2) = 0;
	this->B2_vector(0, 0) = -212.132;
	this->B2_vector(0, 1) = 150;
	this->B2_vector(0, 2) = 0;
	this->B3_vector(0, 0) = 0;
	this->B3_vector(0, 1) = -150;
	this->B3_vector(0, 2) = 0;
}
