#ifndef _MATRIX_CALC_
#define _MATRIX_CALC_

//Eigen
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
	//Result
	MatrixXd L1_vector;
	MatrixXd L2_vector;
	MatrixXd L3_vector;

	Matrix_ball();
	void find_theta(double vector_X, const int Hard_height);
	void find_phi(double vector_Y, const int Hard_height);
	void BR_p_Set();
	void T_vector_Set(double vector_X, double vector_Y, const int Hard_height);
	void P_vector_Set();
	void B_vector_Set();
};

#endif _MATRIX_CALC_