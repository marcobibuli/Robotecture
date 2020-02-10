/*
 * SimStatus.h
 *
 *  Created on: Jan 16, 2018
 *      Author: mx
 */

#ifndef SIMSTATUS_H_
#define SIMSTATUS_H_


#include "../../generic/define.h"
#include "../../generic/RobotThread.h"
#include "../../generic/RobotMath.h"
#include "../../generic/matrix.h"

#define ALTITUDE_ZERO_OFFSET (0.5)
#define SEAFLOOR_DEPTH (40.0)


class SimStatus:public RobotThread
{
	protected:
		double dt;

	public:
		SimStatus(const char* name);
		~SimStatus();

		virtual void execute() = 0;
		Vector strapdown(Vector &v,Matrix &Rold);


		double fu,fv,fw,tp,tq,tr;

		Matrix rotX,rotY,rotZ , J1,J2;
		Matrix M,C,D,Dq,g;
		Matrix eta,nu,eta_dot,nu_dot;
		Matrix tau,F,absNu;

		Vector W,B,rw,rb;

		double u,v,w;
		double x,y,z;

		double p,q,r;
		double phi,theta,psi;

		double xAcc,yAcc,zAcc;
		double pDot,qDot,rDot;
		double xDot,yDot,zDot;

		double vx_sea_current,vy_sea_current;
		double u_sea_current,v_sea_current;

		double altitude;	
};




void* start_simStatus(void* arg);


#endif /* SIMSTATUS_H_ */
