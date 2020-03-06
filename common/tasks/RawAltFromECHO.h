/*
 * RawAltFromECHO.h
 *
 *  Created on: Feb 8, 2019
 *      Author: mx
 */

#ifndef TASKS_RAWALTFROMECHO_H_
#define TASKS_RAWALTFROMECHO_H_


#include "RobotTask.h"
#include "../drivers/Device.h"


class RawAltFromECHO:public RobotTask
{
	private:
		DataAccess<Task_status>* task_access;
		DataAccess<Echologger_status>* echo_access;
		DataAccess<NGC_status>* ngc_access;

	public:
		RawAltFromECHO(const char *name, DataAccess<Task_status>& Task_access, DataAccess<Echologger_status>& ECHO_access, DataAccess<NGC_status>& NGC_access) :RobotTask(name)
		{
			task_access = &Task_access;
			echo_access = &ECHO_access;
			ngc_access = &NGC_access;
		}


		virtual ~RawAltFromECHO(){}

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

			Echologger_status echologger_status;
			NGC_status ngc_status;

			echologger_status = echo_access->get();
			ngc_status = ngc_access->get();

			double phi,theta;
			phi=ngc_status.pose.actual.phi.value;
			theta=ngc_status.pose.actual.theta.value;

			ngc_status.pose.raw.altitude.value=echologger_status.range.value*cos(phi)*cos(theta);

			ngc_status.pose.raw.altitude.valid=true;
			if (echologger_status.device_status!=DEVICE_RUNNING || echologger_status.range.value<0.0)
				ngc_status.pose.raw.altitude.valid=false;


			if (ngc_status.pose.raw.altitude.valid==true) ngc_status.pose.raw.altitude.timeStamp=echologger_status.timeStamp;

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



#endif /* TASKS_RAWALTFROMECHO_H_ */
