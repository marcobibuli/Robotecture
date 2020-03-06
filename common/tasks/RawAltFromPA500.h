/*
 * RawAltFromPA500.h
 *
 *  Created on: Feb 8, 2019
 *      Author: mx
 */

#ifndef TASKS_RAWALTFROMPA500_H_
#define TASKS_RAWALTFROMPA500_H_


#include "RobotTask.h"
#include "../drivers/Device.h"


class RawAltFromPA500:public RobotTask
{
	private:
		DataAccess<Task_status>* task_access;
		DataAccess<PA500_status>* pa500_access;
		DataAccess<NGC_status>* ngc_access;

	public:
		RawAltFromPA500(const char *name, DataAccess<Task_status>& Task_access, DataAccess<PA500_status>& PA500_access, DataAccess<NGC_status>& NGC_access) :RobotTask(name)
		{
			task_access = &Task_access;
			pa500_access = &PA500_access;
			ngc_access = &NGC_access;
		}


		virtual ~RawAltFromPA500(){}

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

			pa500_status = pa500_access->get();
			ngc_status = ngc_access->get();

			double phi,theta;
			phi=ngc_status.pose.actual.phi.value;
			theta=ngc_status.pose.actual.theta.value;

			ngc_status.pose.raw.altitude.value=pa500_status.range.value*cos(phi)*cos(theta);

			ngc_status.pose.raw.altitude.valid=true;
			if (pa500_status.device_status!=DEVICE_RUNNING || pa500_status.range.value<0.0)
				ngc_status.pose.raw.altitude.valid=false;


			if (ngc_status.pose.raw.altitude.valid==true) ngc_status.pose.raw.altitude.timeStamp=pa500_status.timeStamp;

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



#endif /* TASKS_RAWALTFROMPA500_H_ */
