/*
 * NGC_status.h
 *
 *  Created on: Jan 22, 2019
 *      Author: mx
 */

#ifndef NGC_STATUS_H_
#define NGC_STATUS_H_

#include "../generic/custom_types.h"
#include "../generic/Variable.h"
#include "../generic/RobotMath.h"



enum NGC_cmd
{
	SET_FU_REF = 0,
	SET_FV_REF = 1,
	SET_FW_REF = 2,
	SET_TZ_REF = 3
};



# pragma pack (push, 1)
struct NGC_cmd_packet
{
	int32 cmd_type;
	int64 value;
};
# pragma pack (pop)




# pragma pack (push, 1)
struct NGC_tlm_packet
{
	int64 fu,fv,fw,tp,tq,tr;
	int64 u,v,w,p,q,r;
	int64 xDot,yDot,speed,course;
	int64 x,y,z,altitude;
	int64 phi,theta,psi;

	void zero()
	{
		fu=0;     fv=0;     fw=0;    tp=0;  tq=0;    tr=0;
		u=0;      v=0;      w=0;     p=0;   q=0;     r=0;
		xDot=0;   yDot=0;   speed=0; course=0;
		x=0;  y=0;  z=0;
		altitude=0;
	    phi=0;    theta=0;  psi=0;
	}
};
# pragma pack (pop)



class NGC_status
{
	private:

		void copy_data(const NGC_status &d)
		{
			pose=d.pose;
			velocity_body=d.velocity_body;
			velocity_inertial=d.velocity_inertial;
			force=d.force;

			commands = d.commands;

			compose_tlm_packet(tlm_packet);
		}


	public:

		PoseSet pose;
		VelocityBodySet velocity_body;
		VelocityInertialSet velocity_inertial;
		ForceSet force;
		
		std::vector<std::string> commands;

		NGC_tlm_packet tlm_packet;


		NGC_status()
		{
			pose.zero();

			velocity_body.zero();

			velocity_inertial.zero();


			force.zero();


			tlm_packet.zero();

			commands.clear();
		}


		NGC_status(NGC_status &d)
		{
			copy_data(d);
		}


		~NGC_status(){}


		NGC_status& operator=(const NGC_status &d)
		{
			copy_data(d);
			return *this;
		}


		void compose_tlm_packet(NGC_tlm_packet&tp)
		{
			tp.fu=(int64)(force.actual.fu.value*NGC_factor);
			tp.fv=(int64)(force.actual.fv.value*NGC_factor);
			tp.fw=(int64)(force.actual.fw.value*NGC_factor);
			tp.tp=(int64)(force.actual.tp.value*NGC_factor);
			tp.tq=(int64)(force.actual.tq.value*NGC_factor);
			tp.tr=(int64)(force.actual.tr.value*NGC_factor);

			tp.u=(int64)(velocity_body.actual.u.value*NGC_factor);
			tp.v=(int64)(velocity_body.actual.v.value*NGC_factor);
			tp.w=(int64)(velocity_body.actual.w.value*NGC_factor);
			tp.p=(int64)(velocity_body.actual.p.value*(180.0/M_PI)*NGC_factor);
			tp.q=(int64)(velocity_body.actual.q.value*(180.0/M_PI)*NGC_factor);
			tp.r=(int64)(velocity_body.actual.r.value*(180.0/M_PI)*NGC_factor);

			tp.xDot=(int64)(velocity_inertial.actual.xDot.value*NGC_factor);
			tp.yDot=(int64)(velocity_inertial.actual.yDot.value*NGC_factor);
			tp.speed=(int64)(velocity_inertial.actual.speed.value*NGC_factor);
			tp.course=(int64)(velocity_inertial.actual.course.value*(180.0/M_PI)*NGC_factor);

			tp.x=(int64)(pose.actual.x.value*NGC_factor);
			tp.y=(int64)(pose.actual.y.value*NGC_factor);
			tp.z=(int64)(pose.actual.z.value*NGC_factor);
			tp.altitude=(int64)(pose.actual.altitude.value*NGC_factor);

			tp.phi=(int64)(pose.actual.phi.value*(180.0/M_PI)*NGC_factor);
			tp.theta=(int64)(pose.actual.theta.value*(180.0/M_PI)*NGC_factor);
			tp.psi=(int64)(pose.actual.psi.value*(180.0/M_PI)*NGC_factor);
		}


		void compose_string_packet(char* str)
		{	
			sprintf(str, "%.2lf %.2lf %.2lf %.2lf %.2lf", pose.actual.x.value, pose.actual.y.value, pose.actual.z.value, pose.actual.altitude.value, pose.actual.psi.value * (180.0 / M_PI));
		}
};


#endif /* NGC_STATUS_H_ */
