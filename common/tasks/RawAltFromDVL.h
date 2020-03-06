/*
 * RawAltFromDVL.h
 *
 *  Created on: Feb 8, 2019
 *      Author: mx
 */

#ifndef TASKS_RAWALTFROMDVL_H_
#define TASKS_RAWALTFROMDVL_H_


#include "RobotTask.h"
#include "../drivers/Device.h"


class RawAltFromDVL:public RobotTask
{
	private:
		DataAccess<Task_status>* task_access;
		DataAccess<DVL_status>* dvl_access;
		DataAccess<NGC_status>* ngc_access;

	public:
		RawAltFromDVL(const char *name, DataAccess<Task_status>& Task_access, DataAccess<DVL_status>& DVL_access, DataAccess<NGC_status>& NGC_access) :RobotTask(name)
		{
			task_access = &Task_access;
			dvl_access = &DVL_access;
			ngc_access = &NGC_access;
		}


		virtual ~RawAltFromDVL(){}

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

			DVL_status dvl_status;
			NGC_status ngc_status;

			dvl_status = dvl_access->get();
			ngc_status = ngc_access->get();

			double phi,theta;
			phi=ngc_status.pose.actual.phi.value;
			theta=ngc_status.pose.actual.theta.value;

			ngc_status.pose.raw.altitude.value=dvl_status.altitude.value*cos(phi)*cos(theta);

			ngc_status.pose.raw.altitude.valid=true;
			if (dvl_status.device_status!=DEVICE_RUNNING || dvl_status.validVelocity==0)
				ngc_status.pose.raw.altitude.valid=false;


			if (ngc_status.pose.raw.altitude.valid==true) ngc_status.pose.raw.altitude.timeStamp=dvl_status.timeStamp;


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


#endif /* TASKS_RAWALTFROMDVL_H_ */
