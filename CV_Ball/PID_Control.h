/*===================================================================================
 * Ball Detecting with opencv
 * using PID control and send to MCU
 * PID_Control.h
 *
 * Author : �����
===================================================================================*/


#ifndef PID_CONTROL_
#define PID_CONTROL_

class PID_Control {
public:
	long nowValue;		//!< ���� ��
	long pastValue;		//!< ���� ��

	long nowError;		//!< ���� ������
	long pastError;		//!< ���� ������
	long target;		//!< ��ǥ��

	long errorSum;		//!< ���� ������
	long errorSumLimit;	//!< ���� ������ ���� (0�� ��� ���� ����)
	long errorDiff;		//!< ���� �̺а�

	long nowOutput;		//!< ���� ��°�
	long pastOutput;	//!< ���� ��°�
	long outputLimit;	//!< ��� ���Ѱ�

	long underOfPoint;	//!< kP, kI, kD�� �������� ���� ������ (underOfPoint=1000, kP=1�̸� P�̵氪 = 0.001)

	double kP;			//!< P(���)�̵氪
	double kI;			//!< I(����)�̵氪
	double kD;			//!< D(�̺�)�̵氪

	//Initialize
	PID_Control();

	void PID_Control_long(
		long target, 				//!< ��ǥ ��
		long input					//!< ���� ��
	);
};


#endif PID_CONTROL_
