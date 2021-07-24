/*===================================================================================
 * Ball Detecting with opencv
 * using PID control and send to MCU
 * PID_Control.h
 *
 * Author : 이재원
===================================================================================*/


#ifndef PID_CONTROL_
#define PID_CONTROL_

class PID_Control {
public:
	long nowValue;		//!< 현재 값
	long pastValue;		//!< 이전 값

	long nowError;		//!< 현재 에러값
	long pastError;		//!< 이전 에러값
	long target;		//!< 목표값

	long errorSum;		//!< 에러 누적값
	long errorSumLimit;	//!< 에러 누적값 제한 (0일 경우 제한 없음)
	long errorDiff;		//!< 에러 미분값

	long nowOutput;		//!< 현재 출력값
	long pastOutput;	//!< 이전 출력값
	long outputLimit;	//!< 출력 제한값

	long underOfPoint;	//!< kP, kI, kD에 공통으로 들어가는 나눗셈 (underOfPoint=1000, kP=1이면 P이득값 = 0.001)

	double kP;			//!< P(비례)이득값
	double kI;			//!< I(적분)이득값
	double kD;			//!< D(미분)이득값

	//Initialize
	PID_Control();

	void PID_Control_long(
		long target, 				//!< 목표 값
		long input					//!< 현재 값
	);
};


#endif PID_CONTROL_
