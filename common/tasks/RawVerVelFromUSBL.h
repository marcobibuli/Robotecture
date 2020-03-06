/*
 * RawVerVelFromUSBL.h
 *
 *  Created on: Feb 8, 2019
 *      Author: mx
 */

#ifndef TASKS_RAWVERVELFROMUSBL_H_
#define TASKS_RAWVERVELFROMUSBL_H_


#include "RobotTask.h"
#include "../drivers/Device.h"

class RawVerVelFromUSBL:public RobotTask
{
	private:
		double z_prev;
		bool valid;

		DataAccess<Task_status>* task_access;
		DataAccess<Pinger_status>* pinger_access;
		DataAccess<NGC_status>* ngc_access;
		DataAccess<Time_status>* time_access;

	public:
		RawVerVelFromUSBL(const char *name, DataAccess<Task_status>& Task_access, DataAccess<Pinger_status>& Pinger_access, DataAccess<NGC_status>& NGC_access, DataAccess<Time_status>& Time_access) :RobotTask(name)
		{
			task_access = &Task_access;
			pinger_access = &Pinger_access;
			ngc_access = &NGC_access;
			time_access = &Time_access;

			z_prev=0.0;
			valid=false;
		}


		virtual ~RawVerVelFromUSBL(){}

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

			time_status = time_access->get();
			double dt = time_status.dt;

			pinger_status = pinger_access->get();
			ngc_status = ngc_access->get();

			double z= pinger_status.z.value;

			if (!valid)
			{
				z_prev=z;
			}
			valid=true;

			double phi,theta;
			phi=ngc_status.pose.actual.phi.value;
			theta=ngc_status.pose.actual.theta.value;
			double w=(z-z_prev)/dt;
			z_prev=z;

			ngc_status.velocity_body.raw.w.value=w;
			ngc_status.velocity_inertial.raw.zDot.value=w*cos(phi)*cos(theta);

			ngc_status.velocity_body.raw.w.valid=true;
			ngc_status.velocity_inertial.raw.zDot.valid=true;
			if (pinger_status.device_status!=DEVICE_RUNNING)
			{
				ngc_status.velocity_body.raw.w.valid=false;
				ngc_status.velocity_inertial.raw.zDot.valid=false;
			}


			if (ngc_status.velocity_body.raw.w.valid==true) ngc_status.velocity_body.raw.w.timeStamp= pinger_status.timeStamp;
			if (ngc_status.velocity_inertial.raw.zDot.valid==true) ngc_status.velocity_inertial.raw.zDot.timeStamp= pinger_status.timeStamp;



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



#endif /* TASKS_RAWVERVELFROMUSBL_H_ */
