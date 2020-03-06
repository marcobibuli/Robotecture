/*
 * RawVerVelFromPA500.h
 *
 *  Created on: Feb 8, 2019
 *      Author: mx
 */

#ifndef TASKS_RAWVERVELFROMPA500_H_
#define TASKS_RAWVERVELFROMPA500_H_


#include "RobotTask.h"
#include "../drivers/Device.h"

class RawVerVelFromPA500:public RobotTask
{
	private:
		double z_prev;
		bool valid;

		DataAccess<Task_status>* task_access;
		DataAccess<PA500_status>* pa500_access;
		DataAccess<NGC_status>* ngc_access;
		DataAccess<Time_status>* time_access;

	public:
		RawVerVelFromPA500(const char *name, DataAccess<Task_status>& Task_access, DataAccess<PA500_status>& PA500_access, DataAccess<NGC_status>& NGC_access, DataAccess<Time_status>& Time_access) :RobotTask(name)
		{
			task_access = &Task_access;
			pa500_access = &PA500_access;
			ngc_access = &NGC_access;
			time_access = &Time_access;

			z_prev=0.0;
			valid=false;
		}


		virtual ~RawVerVelFromPA500(){}

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
			PA500_status pa500_status;
			NGC_status ngc_status;
			Time_status time_status;

			time_status = time_access->get();
			double dt=time_status.dt;

			pa500_status = pa500_access->get();
			ngc_status = ngc_access->get();

			double z=pa500_status.range.value;

			if (!valid)
			{
				z_prev=z;
			}
			valid=true;

			double phi,theta;
			phi=ngc_status.pose.actual.phi.value;
			theta=ngc_status.pose.actual.theta.value;
			double w=-(z-z_prev)/dt;
			z_prev=z;

			ngc_status.velocity_body.raw.w.value=w;
			ngc_status.velocity_inertial.raw.zDot.value=w*cos(phi)*cos(theta);

			ngc_status.velocity_body.raw.w.valid=true;
			ngc_status.velocity_inertial.raw.zDot.valid=true;
			if (pa500_status.device_status!=DEVICE_RUNNING || pa500_status.range.value<0.0)
			{
				ngc_status.velocity_body.raw.w.valid=false;
				ngc_status.velocity_inertial.raw.zDot.valid=false;
			}


			if (ngc_status.velocity_body.raw.w.valid==true) ngc_status.velocity_body.raw.w.timeStamp=pa500_status.timeStamp;
			if (ngc_status.velocity_inertial.raw.zDot.valid==true) ngc_status.velocity_inertial.raw.zDot.timeStamp=pa500_status.timeStamp;


			ngc_access->set(ngc_status);

		}


		virtual void setStatus(TaskStatus ts)
		{
			Task_status task_status;
			task_status = task_access->get();
			task_status.execution = ts;
			task_access->set(task_status);
		}


		virtual TaskStatus getStatus()
		{
			Task_status task_status;
			task_status = task_access->get();
			return task_status.execution;
		}
};


#endif /* TASKS_RAWVERVELFROMPA500_H_ */
