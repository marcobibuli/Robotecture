/*
 * RawHorVelFromDVL.h
 *
 *  Created on: Feb 8, 2019
 *      Author: mx
 */

#ifndef TASKS_RAWHORVELFROMDVL_H_
#define TASKS_RAWHORVELFROMDVL_H_


#include "RobotTask.h"
#include "../drivers/Device.h"

class RawHorVelFromDVL:public RobotTask
{
	private:

	public:
		RawHorVelFromDVL(const char *name,Status *st):RobotTask(name,st)
		{

		}


		virtual ~RawHorVelFromDVL(){}

		virtual void execute()
		{
			Task_status task_status;
			task_status=status->raw_HorVel_From_DVL_status.get();

			if (task_status.execution==TASK_INIT)
			{
				init();
			}
			if (task_status.execution==TASK_RUNNING)
			{
				compute();
			}
		}

		virtual void compute()
		{

			DVL_status dvl_status;
			NGC_status ngc_status;

			dvl_status=status->dvl_status.get();
			ngc_status=status->ngc_status.get();

			double psi=ngc_status.pose.actual.psi.value;

			double u=0.0 , v=0.0;
			if (dvl_status.validVelocity==1)
			{
				u=dvl_status.surgeVelocity.value;
				v=dvl_status.swayVelocity.value;
			}

			double xDot=cos(psi)*u -sin(psi)*v;
			double yDot=sin(psi)*u +cos(psi)*v;
			double speed=sqrt(xDot*xDot + yDot*yDot);
			double course=atan2(yDot,xDot);

			ngc_status.velocity_body.raw.u.value=u;
			ngc_status.velocity_body.raw.v.value=v;
			ngc_status.velocity_body.raw.u_sea.value=0.0;
			ngc_status.velocity_body.raw.v_sea.value=0.0;
			ngc_status.velocity_inertial.raw.xDot.value=xDot;
			ngc_status.velocity_inertial.raw.yDot.value=yDot;
			ngc_status.velocity_inertial.raw.xDot_sea.value=0.0;
			ngc_status.velocity_inertial.raw.yDot_sea.value=0.0;
			ngc_status.velocity_inertial.raw.speed.value=speed;
			ngc_status.velocity_inertial.raw.course.value=course;


			ngc_status.velocity_body.raw.u.valid=true;
			ngc_status.velocity_body.raw.v.valid=true;
			ngc_status.velocity_body.raw.u_sea.valid=true;
			ngc_status.velocity_body.raw.v_sea.valid=true;
			ngc_status.velocity_inertial.raw.xDot.valid=true;
			ngc_status.velocity_inertial.raw.yDot.valid=true;
			ngc_status.velocity_inertial.raw.xDot_sea.valid=true;
			ngc_status.velocity_inertial.raw.yDot_sea.valid=true;
			ngc_status.velocity_inertial.raw.speed.valid=true;
			ngc_status.velocity_inertial.raw.course.valid=true;
			if (dvl_status.device_status!=DEVICE_RUNNING || dvl_status.validVelocity==0)
			{
				ngc_status.velocity_body.raw.u.valid=false;
				ngc_status.velocity_body.raw.v.valid=false;
				ngc_status.velocity_body.raw.u_sea.valid=false;
				ngc_status.velocity_body.raw.v_sea.valid=false;
				ngc_status.velocity_inertial.raw.xDot.valid=false;
				ngc_status.velocity_inertial.raw.yDot.valid=false;
				ngc_status.velocity_inertial.raw.xDot_sea.valid=false;
				ngc_status.velocity_inertial.raw.yDot_sea.valid=false;
				ngc_status.velocity_inertial.raw.speed.valid=false;
				ngc_status.velocity_inertial.raw.course.valid=false;
			}


			if (ngc_status.velocity_body.raw.u.valid==true) ngc_status.velocity_body.raw.u.timeStamp=dvl_status.timeStamp;
			if (ngc_status.velocity_body.raw.v.valid==true) ngc_status.velocity_body.raw.v.timeStamp=dvl_status.timeStamp;
			if (ngc_status.velocity_body.raw.u_sea.valid==true) ngc_status.velocity_body.raw.u_sea.timeStamp=dvl_status.timeStamp;
			if (ngc_status.velocity_body.raw.v_sea.valid==true) ngc_status.velocity_body.raw.v_sea.timeStamp=dvl_status.timeStamp;
			if (ngc_status.velocity_inertial.raw.xDot.valid==true) ngc_status.velocity_inertial.raw.xDot.timeStamp=dvl_status.timeStamp;
			if (ngc_status.velocity_inertial.raw.yDot.valid==true) ngc_status.velocity_inertial.raw.yDot.timeStamp=dvl_status.timeStamp;
			if (ngc_status.velocity_inertial.raw.xDot_sea.valid==true) ngc_status.velocity_inertial.raw.xDot_sea.timeStamp=dvl_status.timeStamp;
			if (ngc_status.velocity_inertial.raw.yDot_sea.valid==true) ngc_status.velocity_inertial.raw.yDot_sea.timeStamp=dvl_status.timeStamp;
			if (ngc_status.velocity_inertial.raw.speed.valid==true) ngc_status.velocity_inertial.raw.speed.timeStamp=dvl_status.timeStamp;
			if (ngc_status.velocity_inertial.raw.course.valid==true) ngc_status.velocity_inertial.raw.course.timeStamp=dvl_status.timeStamp;

			status->ngc_status.set(ngc_status);

		}


		virtual void setStatus(TaskStatus ts)
		{
			Task_status task_status;
			task_status=status->raw_HorVel_From_DVL_status.get();
			task_status.execution=ts;
			status->raw_HorVel_From_DVL_status.set(task_status);
		}


		virtual TaskStatus getStatus()
		{
			Task_status task_status;
			task_status=status->raw_HorVel_From_DVL_status.get();
			return task_status.execution;
		}
};



#endif /* TASKS_RAWHORVELFROMDVL_H_ */
