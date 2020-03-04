/*
 * RawHorVelFromGPS.h
 *
 *  Created on: Feb 8, 2019
 *      Author: mx
 */

#ifndef TASKS_RAWHORVELFROMGPS_H_
#define TASKS_RAWHORVELFROMGPS_H_


#include "RobotTask.h"
#include "../drivers/Device.h"

class RawHorVelFromGPS:public RobotTask
{
	private:
		DataAccess<Task_status>* task_access;
		DataAccess<GPS_AHRS_status>* gps_ahrs_access;
		DataAccess<NGC_status>* ngc_access;

	public:
		RawHorVelFromGPS(const char* name, DataAccess<Task_status>& Task_access, DataAccess<GPS_AHRS_status>& GPS_AHRS_access, DataAccess<NGC_status>& NGC_access) :RobotTask(name)
		{
			task_access = &Task_access;
			gps_ahrs_access = &GPS_AHRS_access;
			ngc_access = &NGC_access;
		}


		virtual ~RawHorVelFromGPS(){}

		virtual void execute()
		{
			Task_status task_status;
			task_status = task_access->get();

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

			GPS_AHRS_status gps_ahrs_status;
			NGC_status ngc_status;

			gps_ahrs_status=gps_ahrs_access->get();
			ngc_status=ngc_access->get();


			double psi=ngc_status.pose.actual.psi.value;

			double xDot=0.0 , yDot=0.0;

			if (gps_ahrs_status.nedVelocityValidFlags!=0)
			{
				xDot=gps_ahrs_status.northVel.value;
				yDot=gps_ahrs_status.eastVel.value;
			}

			double u=cos(psi)*xDot +sin(psi)*yDot;
			double v=-sin(psi)*xDot +cos(psi)*yDot;

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
			if (gps_ahrs_status.gps_status!=DEVICE_RUNNING || gps_ahrs_status.nedVelocityValidFlags==0)
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


			if (ngc_status.velocity_body.raw.u.valid==true) ngc_status.velocity_body.raw.u.timeStamp=gps_ahrs_status.timeStamp;
			if (ngc_status.velocity_body.raw.v.valid==true) ngc_status.velocity_body.raw.v.timeStamp=gps_ahrs_status.timeStamp;
			if (ngc_status.velocity_body.raw.u_sea.valid==true) ngc_status.velocity_body.raw.u_sea.timeStamp=gps_ahrs_status.timeStamp;
			if (ngc_status.velocity_body.raw.v_sea.valid==true) ngc_status.velocity_body.raw.v_sea.timeStamp=gps_ahrs_status.timeStamp;
			if (ngc_status.velocity_inertial.raw.xDot.valid==true) ngc_status.velocity_inertial.raw.xDot.timeStamp=gps_ahrs_status.timeStamp;
			if (ngc_status.velocity_inertial.raw.yDot.valid==true) ngc_status.velocity_inertial.raw.yDot.timeStamp=gps_ahrs_status.timeStamp;
			if (ngc_status.velocity_inertial.raw.xDot_sea.valid==true) ngc_status.velocity_inertial.raw.xDot_sea.timeStamp=gps_ahrs_status.timeStamp;
			if (ngc_status.velocity_inertial.raw.yDot_sea.valid==true) ngc_status.velocity_inertial.raw.yDot_sea.timeStamp=gps_ahrs_status.timeStamp;
			if (ngc_status.velocity_inertial.raw.speed.valid==true) ngc_status.velocity_inertial.raw.speed.timeStamp=gps_ahrs_status.timeStamp;
			if (ngc_status.velocity_inertial.raw.course.valid==true) ngc_status.velocity_inertial.raw.course.timeStamp=gps_ahrs_status.timeStamp;



			ngc_access->set(ngc_status);

		}


		virtual void setStatus(TaskStatus ts)
		{
			Task_status task_status;
			task_status = task_access->get();
			task_status.execution=ts;
			task_access->set(task_status);
		}


		virtual TaskStatus getStatus()
		{
			Task_status task_status;
			task_status = task_access->get();
			return task_status.execution;
		}
};



#endif /* TASKS_RAWHORVELFROMGPS_H_ */
