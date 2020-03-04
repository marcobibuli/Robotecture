/*
 * RawHorVelFromUSBL.h
 *
 *  Created on: Feb 8, 2019
 *      Author: mx
 */

#ifndef TASKS_RAWHORVELFROMUSBL_H_
#define TASKS_RAWHORVELFROMUSBL_H_


#include "RobotTask.h"
#include "../drivers/Device.h"

class RawHorVelFromUSBL:public RobotTask
{
	private:
		double x_prev;
		double y_prev;
		bool valid;

		DataAccess<Task_status>* task_access;
		DataAccess<Pinger_status>* pinger_access;
		DataAccess<NGC_status>* ngc_access;
		DataAccess<Time_status>* time_access;

	public:
		RawHorVelFromUSBL(const char* name, DataAccess<Task_status>& Task_access, DataAccess<Pinger_status>& Pinger_access, DataAccess<NGC_status>& NGC_access, DataAccess<Time_status>& Time_access) :RobotTask(name)
		{
			task_access = &Task_access;
			pinger_access = &Pinger_access;
			ngc_access = &NGC_access;
			time_access = &Time_access;

			x_prev=0.0;
			y_prev=0.0;
			valid=false;
		}


		virtual ~RawHorVelFromUSBL(){}

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
			Pinger_status pinger_status;
			NGC_status ngc_status;
			Time_status time_status;

			time_status=time_access->get();
			pinger_status =pinger_access->get();
			ngc_status= ngc_access->get();

			double dt=time_status.dt;

			bool valid_measure=false;
			double x=0.0 , y=0.0;
			if (pinger_status.device_status==DEVICE_RUNNING)
			{
				x= pinger_status.x.value;
				y= pinger_status.y.value;
				valid_measure=true;
			}
			double psi=ngc_status.pose.actual.psi.value;

			if (!valid)
			{
				x_prev=x;
				y_prev=y;
			}

			valid=true;

			double xDot=(x-x_prev)/dt;
			double yDot=(y-y_prev)/dt;
			double u=cos(psi)*xDot +sin(psi)*yDot;
			double v=-sin(psi)*xDot +cos(psi)*yDot;
			double speed=sqrt(xDot*xDot + yDot*yDot);
			double course=atan2(yDot,xDot);

			x_prev=x;
			y_prev=y;

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


			ngc_status.velocity_body.raw.u.valid=valid_measure;
			ngc_status.velocity_body.raw.v.valid=valid_measure;
			ngc_status.velocity_body.raw.u_sea.valid=valid_measure;
			ngc_status.velocity_body.raw.v_sea.valid=valid_measure;
			ngc_status.velocity_inertial.raw.xDot.valid=valid_measure;
			ngc_status.velocity_inertial.raw.yDot.valid=valid_measure;
			ngc_status.velocity_inertial.raw.xDot_sea.valid=valid_measure;
			ngc_status.velocity_inertial.raw.yDot_sea.valid=valid_measure;
			ngc_status.velocity_inertial.raw.speed.valid=valid_measure;
			ngc_status.velocity_inertial.raw.course.valid=valid_measure;



			if (ngc_status.velocity_body.raw.u.valid==true) ngc_status.velocity_body.raw.u.timeStamp= pinger_status.timeStamp;
			if (ngc_status.velocity_body.raw.v.valid==true) ngc_status.velocity_body.raw.v.timeStamp= pinger_status.timeStamp;
			if (ngc_status.velocity_body.raw.u_sea.valid==true) ngc_status.velocity_body.raw.u_sea.timeStamp= pinger_status.timeStamp;
			if (ngc_status.velocity_body.raw.v_sea.valid==true) ngc_status.velocity_body.raw.v_sea.timeStamp= pinger_status.timeStamp;
			if (ngc_status.velocity_inertial.raw.xDot.valid==true) ngc_status.velocity_inertial.raw.xDot.timeStamp= pinger_status.timeStamp;
			if (ngc_status.velocity_inertial.raw.yDot.valid==true) ngc_status.velocity_inertial.raw.yDot.timeStamp= pinger_status.timeStamp;
			if (ngc_status.velocity_inertial.raw.xDot_sea.valid==true) ngc_status.velocity_inertial.raw.xDot_sea.timeStamp= pinger_status.timeStamp;
			if (ngc_status.velocity_inertial.raw.yDot_sea.valid==true) ngc_status.velocity_inertial.raw.yDot_sea.timeStamp= pinger_status.timeStamp;
			if (ngc_status.velocity_inertial.raw.speed.valid==true) ngc_status.velocity_inertial.raw.speed.timeStamp= pinger_status.timeStamp;
			if (ngc_status.velocity_inertial.raw.course.valid==true) ngc_status.velocity_inertial.raw.course.timeStamp= pinger_status.timeStamp;


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



#endif /* TASKS_RAWHORVELFROMUSBL_H_ */
