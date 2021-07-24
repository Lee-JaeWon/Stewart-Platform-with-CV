/*===================================================================================
 * Ball Detecting with opencv
 * using PID control and send to MCU
 * PID_Control.cpp
 *
 * Author : 이재원
===================================================================================*/

#include "PID_Control.h"

PID_Control::PID_Control() {
	this->errorSum = 0;
	this->errorSumLimit = 0;

	this->kP = 700;		//6000
	this->kI = 40;		//1200
	this->kD = 40;

	this->pastError = 0;
	this->pastOutput = 0;
	this->pastValue = 0;

	this->underOfPoint = 1000;

	this->outputLimit = 0;
}

void PID_Control::PID_Control_long(long target, long input) {
	this->nowValue = input;
	this->target = target;
	this->nowError = this->nowValue - this->target;
	this->errorSum += this->nowError;
	this->errorDiff = this->nowError - this->pastError;
	
	//에러 누적값 제한
	if (this->errorSumLimit != 0)
	{
		if (this->errorSum > this->errorSumLimit)
			this->errorSum = this->errorSumLimit;
		else if (this->errorSum < -this->errorSumLimit)
			this->errorSum = -this->errorSumLimit;
	}
	this->nowOutput =
		this->kP * this->nowError +
		this->kI * this->errorSum +
		this->kD * this->errorDiff;
	if (this->underOfPoint == 0) return;	// Escape Error
	this->nowOutput /= this->underOfPoint;
	this->pastError = this->nowError;

	//출력 제한값
	if (this->outputLimit != 0)
	{
		if (this->nowOutput > this->outputLimit) this->nowOutput = this->outputLimit;
		else if (this->nowOutput < -this->outputLimit) this->nowOutput = -this->outputLimit;
	}
}