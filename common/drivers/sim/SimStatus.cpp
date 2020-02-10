/*
 * SimStatus.cpp
 *
 *  Created on: Feb 06, 2020
 *      Author: mx
 */

#include "SimStatus.h"

SimStatus::SimStatus(const char *name):RobotThread(name,SCHED_FIFO,STATUS_THREAD_PRIORITY,start_simStatus),
rotX(3,3),rotY(3,3),rotZ(3,3),J1(3,3),J2(3,3),M(6,6),C(6,6),D(6,6),Dq(6,6),g(6,1),
eta(6,1),nu(6,1),eta_dot(6,1),nu_dot(6,1),tau(6,1),F(6,6),absNu(6,6),W(),B(),rw(),rb()
{
	
	fu=0.0; fv=0.0; fw=0.0; tp=0.0; tq=0.0; tr=0.0;


	altitude=0.0;
	p=0.0;     q=0.0;     r=0.0;
	pDot=0.0;  qDot=0.0;  rDot=0.0;
	phi=0.0;   theta=0.0; psi=0.0;

	u=0.0;     v=0.0;     w=0.0;
	x=0.0;     y=0.0;     z=0.0;
	xDot=0.0;  yDot=0.0;  zDot=0.0;
	xAcc=0.0;  yAcc=0.0;  zAcc=0.0;

	vx_sea_current=0.0;
	vy_sea_current=0.0;

	u_sea_current=0.0;
	v_sea_current=0.0;

	dt = 0.1;
}


SimStatus::~SimStatus()
{

}



Vector SimStatus::strapdown(Vector& v, Matrix& Rold)
{
	double gimbal_lock_thr = 0.0000001;
	double r, p, y;

	Vector eta_out;
	Matrix S(3, 3);
	Matrix R(3, 3);
	Matrix I(3, 3);
	I.eye();

	S(0, 0) = 0.0;		S(0, 1) = -v(2);	S(0, 2) = v(1);
	S(1, 0) = v(2);	S(1, 1) = 0.0;		S(1, 2) = -v(0);
	S(2, 0) = -v(1);	S(2, 1) = v(0);	S(2, 2) = 0.0;

	R = Rold * (I + S * dt);

	if (fabs(R(0, 0)) < gimbal_lock_thr && fabs(R(1, 0)) < gimbal_lock_thr)
	{
		y = 0;
		p = -M_PI / 2.0 * R(2, 0);
		r = atan2(-R(0, 1), R(1, 1));
	}

	else
	{
		y = atan2(R(1, 0), R(0, 0));

		double sr = sin(y);
		double cr = cos(y);

		p = -atan2(R(2, 0), (R(0, 0) * cr + R(1, 0) * sr));

		r = atan2((R(0, 2) * sr - R(1, 2) * cr), (-R(0, 1) * sr + R(1, 1) * cr));
	}


	eta_out(0) = r; eta_out(1) = p; eta_out(2) = y;

	return eta_out;
}




void* start_simStatus(void *arg)
{
	SimStatus* pThread=(SimStatus*)((ThreadInfo*)arg)->pThread;
	pThread->execute();
	return NULL;
}

