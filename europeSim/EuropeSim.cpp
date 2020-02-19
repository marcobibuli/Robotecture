/*
 * SimStatus.cpp
 *
 *  Created on: Feb 06, 2020
 *      Author: mx
 */

#include "EuropeSim.h"

EuropeSim::EuropeSim(const char *name):SimStatus(name),
versor(3,8),pos(3,8),force(6,1)
{
	ang_motor=M_PI/4.0;


	versor(0,0)=0.0; 			  	versor(1,0)=0.0; 				versor(2,0)=1.0;  // VFL
	versor(0,1)=0.0; 			  	versor(1,1)=0.0; 				versor(2,1)=1.0;  // VFR
	versor(0,2)=0.0; 			  	versor(1,2)=0.0; 				versor(2,2)=1.0;  // VRL
	versor(0,3)=0.0; 			  	versor(1,3)=0.0; 				versor(2,3)=1.0;  // VRR

	versor(0,4)= cos(ang_motor);	versor(1,4)= sin(ang_motor);	versor(2,4)=0.0;  // HFL
	versor(0,5)= cos(ang_motor); 	versor(1,5)=-sin(ang_motor); 	versor(2,5)=0.0;  // HFR
	versor(0,6)=-cos(ang_motor); 	versor(1,6)=sin(ang_motor); 	versor(2,6)=0.0;  // HRL
	versor(0,7)=-cos(ang_motor); 	versor(1,7)=-sin(ang_motor); 	versor(2,7)=0.0;  // HRR


	pos(0,0)= 0.4; 	pos(1,0)=-0.2; 	pos(2,0)=-0.4;  // VFL
	pos(0,1)= 0.4; 	pos(1,1)= 0.2; 	pos(2,1)=-0.4;  // VFR
	pos(0,2)=-0.4; 	pos(1,2)=-0.2; 	pos(2,2)=-0.4;  // VRL
	pos(0,3)=-0.4; 	pos(1,3)= 0.2; 	pos(2,3)=-0.4;  // VRR

	pos(0,4)= 0.4;	pos(1,4)=-0.2;	pos(2,4)= 0.0;  // HFL
	pos(0,5)= 0.4; 	pos(1,5)= 0.2; 	pos(2,5)= 0.0;  // HFR
	pos(0,6)=-0.4; 	pos(1,6)=-0.2; 	pos(2,6)= 0.0;  // HRL
	pos(0,7)=-0.4; 	pos(1,7)= 0.2; 	pos(2,7)= 0.0;  // HRR


	a_pos=0.0543;
	b_pos=0.0032;

	//a_neg=-0.0654;
	//b_neg=-0.1090;

	a_neg=-a_pos;
	b_neg=-b_pos;


	Svfl=0; Svfr=0; Svrl=0; Svrr=0; Shfl=0; Shfr=0; Shrl=0; Shrr=0;
	Vvfl=0.0; Vvfr=0.0; Vvrl=0.0; Vvrr=0.0; Vhfl=0.0; Vhfr=0.0; Vhrl=0.0; Vhrr=0.0;

	


	port_bow_thr_power = 0;
	port_stern_thr_power = 0;
	stbd_bow_thr_power = 0;
	stbd_stern_thr_power = 0;

	port_bow_thr_enable = 0;
	port_stern_thr_enable = 0;
	stbd_bow_thr_enable = 0;
	stbd_stern_thr_enable = 0;

	port_bow_azm_power = 0;
	port_stern_azm_power = 0;
	stbd_bow_azm_power = 0;
	stbd_stern_azm_power = 0;

	port_bow_azm_enable = 0;
	port_stern_azm_enable = 0;
	stbd_bow_azm_enable = 0;
	stbd_stern_azm_enable = 0;


	port_bow_thr_set = 0;
	port_stern_thr_set = 0;
	stbd_bow_thr_set = 0;
	stbd_stern_thr_set = 0;

	port_bow_azm_set = 0;
	port_stern_azm_set = 0;
	stbd_bow_azm_set = 0;
	stbd_stern_azm_set = 0;


}


EuropeSim::~EuropeSim()
{

}


void EuropeSim::execute()
{
	timespec tSleep;
	tSleep.tv_sec=CORE_SAMPLE_TIME_SEC;
	tSleep.tv_nsec=CORE_SAMPLE_TIME_NSEC;


	Vector g1,g2;

	Vector nu1,nu2,nu_sea_current;

	Vector eta1_dot,eta2_dot,eta_dot_sea_current;

	Vector eta1,eta2;


	eta.zeros();  eta(2,0)=0.0;
	eta(0,0)=0.0; eta(1,0)=0.0;

	M.eye();  C.zeros();  D.eye();  F.eye();  absNu.zeros();

	M(0,0)=100.0;  M(1,1)=100.0;  M(2,2)=100.0;  M(3,3)=20.5;  M(4,4)=32.5;  M(5,5)=18.9;

	D(0,0)=21.98;  D(1,1)=29.68;  D(2,2)=30.0;   D(3,3)=18.0;  D(4,4)=22.0;  D(5,5)=20.5;

	Dq(0,0)=0.0;   Dq(1,1)=0.0;   Dq(2,2)=0.0;   Dq(3,3)=0.0;  Dq(4,4)=0.0;  Dq(5,5)=0.0;


	F(0,0)=0.44;   F(1,1)=0.28;
	F(2,2)=1.0; // not verified
	F(3,3)=0.1; // not verified
	F(4,4)=0.1; // not verified
	F(5,5)=0.89;


	W(0)=0.0; W(1)=0.0; W(2)=100.0;

	B(0)=0.0; B(1)=0.0; B(2)=100.0;

	rw(0)=0.0; rw(1)=0.0; rw(2)=0.0;

	rb(0)=0.0; rb(1)=0.0; rb(2)=0.5;

	tau.zeros();

	
	
	while(true)
	{
		Matrix vref(8, 1);
		vref(0, 0) = Vvfl; vref(1, 0) = Vvfr; vref(2, 0) = Vvrl; vref(3, 0) = Vvrr;
		vref(4, 0) = Vhfl; vref(5, 0) = Vhfr; vref(6, 0) = Vhrl; vref(7, 0) = Vhrr;
		computeForceTorque(vref);

		tr = 1.0;
		fu = 10.0;

		//printf("hfl: %lf   hfr: %lf   hrl: %lf   hrr: %lf\n",Vhfl,Vhfr,Vhrl,Vhrr);
		//printf("fu: %lf   fv: %lf   fw: %lf   tx: %lf   ty: %lf   tz: %lf\n",fu,fv,fw,tx,ty,tz);

		tau(0, 0) = fu; tau(1, 0) = fv; tau(2, 0) = fw; tau(3, 0) = tp; tau(4, 0) = tq; tau(5, 0) = tr;

		eta1(0) = eta(0, 0); eta1(1) = eta(1, 0); eta1(2) = eta(2, 0);
		eta2(0) = eta(3, 0); eta2(1) = eta(4, 0); eta2(2) = eta(5, 0);

		phi = eta(3, 0); theta = eta(4, 0); psi = eta(5, 0);


		rotX(0, 0) = 1.0; 	rotX(0, 1) = 0.0;		rotX(0, 2) = 0.0;
		rotX(1, 0) = 0.0;	rotX(1, 1) = cos(phi);	rotX(1, 2) = -sin(phi);
		rotX(2, 0) = 0.0;	rotX(2, 1) = sin(phi);	rotX(2, 2) = cos(phi);

		rotY(0, 0) = cos(theta);	rotY(0, 1) = 0.0;	rotY(0, 2) = sin(theta);
		rotY(1, 0) = 0.0;			rotY(1, 1) = 1.0;	rotY(1, 2) = 0.0;
		rotY(2, 0) = -sin(theta);	rotY(2, 1) = 0.0;	rotY(2, 2) = cos(theta);

		rotZ(0, 0) = cos(psi);	rotZ(0, 1) = -sin(psi);	rotZ(0, 2) = 0.0;
		rotZ(1, 0) = sin(psi);	rotZ(1, 1) = cos(psi);		rotZ(1, 2) = 0.0;
		rotZ(2, 0) = 0.0;		rotZ(2, 1) = 0.0;			rotZ(2, 2) = 1.0;

		J1 = rotZ * rotY * rotX;




		J2(0, 0) = 1.0;	J2(0, 1) = sin(phi) * tan(theta);	J2(0, 2) = cos(phi) * tan(theta);
		J2(1, 0) = 0.0;	J2(1, 1) = cos(phi);				J2(1, 2) = -sin(phi);
		J2(2, 0) = 0.0;	J2(2, 1) = sin(phi) / cos(theta);	J2(2, 2) = cos(phi) / cos(theta);


		g1 = (J1.inv()) * W - (J1.inv()) * B;
		g2 = rw % (J1.inv() * W) - rb % (J1.inv() * B);
		g(0, 0) = g1(0); g(1, 0) = g1(1); g(2, 0) = g1(2); g(3, 0) = g2(0); g(4, 0) = g2(1); g(5, 0) = g2(2);

		//absNu(0,0)=fabs(nu(0,0)); absNu(1,1)=fabs(nu(1,1)); absNu(2,2)=fabs(nu(2,2));
		//absNu(3,3)=fabs(nu(3,3)); absNu(4,4)=fabs(nu(4,4)); absNu(5,5)=fabs(nu(5,5));

		nu_dot = M.inv() * ((-1.0) * C * nu + (-1.0) * D * nu + (-1.0) * Dq * absNu * nu + (-1.0) * g + F * tau);

		nu = nu + nu_dot * dt;

		nu1(0) = nu(0, 0); nu1(1) = nu(1, 0); nu1(2) = nu(2, 0);
		nu2(0) = nu(3, 0); nu2(1) = nu(4, 0); nu2(2) = nu(5, 0);

		u = nu(0, 0);		v = nu(1, 0);		w = nu(2, 0);
		p = nu(3, 0);		q = nu(4, 0);		r = nu(5, 0);


		eta1_dot = J1 * nu1;
		eta2_dot = J2 * nu2;


		eta1 = eta1 + eta1_dot * dt;
		eta2 = strapdown(eta2_dot, J1);


		eta(0, 0) = eta1(0); eta(1, 0) = eta1(1); eta(2, 0) = eta1(2);
		eta(3, 0) = eta2(0); eta(4, 0) = eta2(1); eta(5, 0) = eta2(2);

		eta_dot(0, 0) = eta1_dot(0); eta_dot(1, 0) = eta1_dot(1); eta_dot(2, 0) = eta1_dot(2);
		eta_dot(3, 0) = eta2_dot(0); eta_dot(4, 0) = eta2_dot(1); eta_dot(5, 0) = eta2_dot(2);

		if (eta(2, 0) < ALTITUDE_ZERO_OFFSET) eta(2, 0) = ALTITUDE_ZERO_OFFSET; //sea surface - remember to change ctd and altimeter mounting position!
		if (eta(2, 0) > (SEAFLOOR_DEPTH - ALTITUDE_ZERO_OFFSET)) eta(2, 0) = SEAFLOOR_DEPTH - ALTITUDE_ZERO_OFFSET; //sea floor - remember to change ctd and altimeter mounting position!

		x = eta(0, 0); y = eta(1, 0); z = eta(2, 0);
		phi = eta(3, 0); theta = eta(4, 0); psi = eta(5, 0);


		altitude = SEAFLOOR_DEPTH - z;


		xAcc = nu_dot(0, 0); yAcc = nu_dot(1, 0); zAcc = nu_dot(2, 0); pDot = nu_dot(3, 0); qDot = nu_dot(4, 0); rDot = nu_dot(5, 0);
		p = nu(3, 0); q = nu(4, 0); r = nu(5, 0);

		xDot = eta_dot(0, 0); yDot = eta_dot(1, 0); zDot = eta_dot(2, 0);

		//printf("x: %lf   y: %lf   z: %lf      phi: %lf   theta: %lf   psi: %lf\n", x, y, z, phi*180.0/M_PI, theta * 180.0 / M_PI, psi * 180.0 / M_PI);

		nanosleep(&tSleep,NULL);
	}
}


void EuropeSim::computeForceTorque(Matrix v)
{
	for(int i=0;i<6;i++)
		force(i,0)=0.0;

	Vector f;
	for(int j=0;j<8;j++)
	{
		Vector vrs( versor(0,j) , versor(1,j) , versor(2,j) );
		f += vrs * thrust_curve(v(j,0)) ;
	}

	Vector t,tt,vv;
	for(int j=0;j<8;j++)
	{
		Vector vrs( versor(0,j) , versor(1,j) , versor(2,j) );
		Vector p( pos(0,j) , pos(1,j) , pos(2,j) );
		vv=( vrs * thrust_curve(v(j,0)) );
		tt=p % vv;
		t += tt;

		//printf("%lf  %lf  %lf %lf\n",thrust_curve(v(4,0)),thrust_curve(v(5,0)),thrust_curve(v(6,0)),thrust_curve(v(7,0)));
	}

	fu=f(0);  fv=f(1);  fw=f(2);
	tp=t(0);  tq=t(1);  tr=t(2);
}


double EuropeSim::thrust_curve(double v)
{
	double th=0.0;

	double vv=fabs(v);

	if (v>=0.0) th=a_pos*vv*vv + b_pos*vv;
	else th=a_neg*vv*vv + b_neg*vv;

	return th;
}





