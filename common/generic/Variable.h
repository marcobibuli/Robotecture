/*
 * Data_status.h
 *
 *  Created on: Jan 21, 2019
 *      Author: mx
 */

#ifndef VARIABLE_H_
#define VARIABLE_H_


#include "string.h"
#include "custom_types.h"


struct Variable
{
	char name[256];
	double value;
	double std;
	uint64 timeStamp;
	bool valid;

	Variable& operator=(const Variable& p)
	{
		strcpy(name, p.name);
		value=p.value;
		std=p.std;
		timeStamp=p.timeStamp;
		valid=p.valid;

		return *this;
	}

	void zero()
	{
		strcpy(name, ""); value = 0.0;  std = 0.0; timeStamp = 0; valid = false;
	}
};



struct Force
{
	Variable fu, fv, fw, tp, tq, tr;

	Force& operator=(const Force& p)
	{
		fu = p.fu;  fv = p.fv;  fw = p.fw;
		tp = p.tp;  tq = p.tq;  tr = p.tr;
		return *this;
	}

	void zero()
	{
		fu.zero();  fv.zero();  fw.zero();
		tp.zero();  tq.zero();  tr.zero();
	}
};




struct ForceSet
{
	Force manual;
	Force automatic;
	Force external;
	Force reference;
	Force actual;

	ForceSet& operator=(const ForceSet& p)
	{
		manual = p.manual;
		automatic = p.automatic;
		external = p.external;
		reference = p.reference;
		actual = p.actual;
		return *this;
	}

	void zero()
	{
		manual.zero();
		automatic.zero();
		external.zero();
		reference.zero();
		actual.zero();
	}
};




struct VelocityBodyFrame
{
	Variable u, v, w, p, q, r, u_sea, v_sea;

	VelocityBodyFrame& operator=(const VelocityBodyFrame& pp)
	{
		u = pp.u;  v = pp.v;  w = pp.w;
		p = pp.p;  q = pp.q;  r = pp.r;
		u_sea = pp.u_sea;  v_sea = pp.v_sea;
		return *this;
	}

	void zero()
	{
		u.zero();  v.zero();  w.zero();
		p.zero();  q.zero();  r.zero();
		u_sea.zero(); v_sea.zero();
	}
};




struct VelocityBodySet
{
	VelocityBodyFrame manual;
	VelocityBodyFrame automatic;
	VelocityBodyFrame external;
	VelocityBodyFrame reference;
	VelocityBodyFrame raw;
	VelocityBodyFrame filtered;
	VelocityBodyFrame actual;

	VelocityBodySet& operator=(const VelocityBodySet& p)
	{
		manual = p.manual;
		automatic = p.automatic;
		external = p.external;
		reference = p.reference;
		raw = p.raw;
		filtered = p.filtered;
		actual = p.actual;
		return *this;
	}

	void zero()
	{
		manual.zero();
		automatic.zero();
		external.zero();
		reference.zero();
		raw.zero();
		filtered.zero();
		actual.zero();
	}
};






struct VelocityInertialFrame
{
	Variable xDot, yDot, zDot, phiDot, thetaDot, psiDot, speed, course, xDot_sea, yDot_sea;

	VelocityInertialFrame& operator=(const VelocityInertialFrame& p)
	{
		xDot = p.xDot;      yDot = p.yDot;          zDot = p.zDot;
		phiDot = p.phiDot;  thetaDot = p.thetaDot;  psiDot = p.psiDot;
		speed = p.speed;    course = p.course;
		xDot_sea = p.xDot_sea;    yDot_sea = p.yDot_sea;
		return *this;
	}

	void zero()
	{
		xDot.zero();    yDot.zero();      zDot.zero();
		phiDot.zero();  thetaDot.zero();  psiDot.zero();
		speed.zero();   course.zero();
		xDot_sea.zero();   yDot_sea.zero();
	}
};





struct VelocityInertialSet
{
	VelocityInertialFrame manual;
	VelocityInertialFrame automatic;
	VelocityInertialFrame external;
	VelocityInertialFrame reference;
	VelocityInertialFrame raw;
	VelocityInertialFrame filtered;
	VelocityInertialFrame actual;

	VelocityInertialSet& operator=(const VelocityInertialSet& p)
	{
		manual = p.manual;
		automatic = p.automatic;
		external = p.external;
		reference = p.reference;
		raw = p.raw;
		filtered = p.filtered;
		actual = p.actual;
		return *this;
	}

	void zero()
	{
		manual.zero();
		automatic.zero();
		external.zero();
		reference.zero();
		raw.zero();
		filtered.zero();
		actual.zero();
	}
};





struct Pose
{
	Variable x, y, z, altitude;
	Variable phi, theta, psi;

	Pose& operator=(const Pose& p)
	{
		x = p.x;      y = p.y;          z = p.z;      altitude = p.altitude;
		phi = p.phi;  theta = p.theta;  psi = p.psi;
		return *this;
	}

	void zero()
	{
		x.zero();    y.zero();      z.zero();    altitude.zero();
		phi.zero();  theta.zero();  psi.zero();
	}
};




struct PoseSet
{
	Pose reference;
	Pose manual;
	Pose automatic;
	Pose external;
	Pose raw;
	Pose filtered;
	Pose actual;

	PoseSet& operator=(const PoseSet& p)
	{
		reference = p.reference;
		manual = p.manual;
		automatic = p.automatic;
		external = p.external;
		raw = p.raw;
		filtered = p.filtered;
		actual = p.actual;
		return *this;
	}

	void zero()
	{
		reference.zero();
		automatic.zero();
		external.zero();
		raw.zero();
		filtered.zero();
		actual.zero();
	}
};






#endif /* VARIABLE_H_ */
