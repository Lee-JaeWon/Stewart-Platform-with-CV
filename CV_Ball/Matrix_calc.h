/*===================================================================================
 * Ball Detecting with opencv
 * using PID control and send to MCU
 * Matrix Calculation for Inverse Kinematics
 * Matrix_calc.h
 *
 * Author : ÀÌÀç¿ø
===================================================================================*/

#ifndef _MATRIX_CALC_
#define _MATRIX_CALC_

//Eigen Library
#include <Eigen/Dense>
#include <cmath>
using namespace Eigen;

class Matrix_ball {
public:
	//theta, phi
	double theta;
	double phi;
	//process
	MatrixXd BR_p;
	MatrixXd T_vector;
	MatrixXd P1_vector;
	MatrixXd P2_vector;
	MatrixXd P3_vector;
	MatrixXd B1_vector;
	MatrixXd B2_vector;
	MatrixXd B3_vector;
	//Result(vector)
	MatrixXd L1_vector;
	MatrixXd L2_vector;
	MatrixXd L3_vector;
	//Result(Scalar)
	double L1_Size;
	double L2_Size;
	double L3_Size;

	//Initiallize
	Matrix_ball();
	void find_theta(double vector_X, const int Hard_height);
	void find_phi(double vector_Y, const int Hard_height);
	void BR_p_Set();
	void T_vector_Set(double vector_X, double vector_Y, const int Hard_height);
	void P_vector_Set();
	const void B_vector_Set();
	double vector_size(double x, double y, double z);
};

#endif _MATRIX_CALC_