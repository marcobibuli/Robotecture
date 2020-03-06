/*
 * RawVerPosFromUSBL.h
 *
 *  Created on: Feb 8, 2019
 *      Author: mx
 */

#ifndef TASKS_RAWVERPOSFROMUSBL_H_
#define TASKS_RAWVERPOSFROMUSBL_H_


#include "RobotTask.h"
#include "../drivers/Device.h"


class RawVerPosFromUSBL:public RobotTask
{
	private:
		DataAccess<Task_status>* task_access;
		DataAccess<Pinger_status>* pinger_access;
		DataAccess<NGC_status>* ngc_access;

	public:
		RawVerPosFromUSBL(const char *name, DataAccess<Task_status>& Task_access, DataAccess<Pinger_status>& Pinger_access, DataAccess<NGC_status>& NGC_access) :RobotTask(name)
		{
			task_access = &Task_access;
			pinger_access = &Pinger_access;
			ngc_access = &NGC_access;
		}


		virtual ~RawVerPosFromUSBL(){}

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

			pinger_status = pinger_access->get();
			ngc_status = ngc_access->get();

			ngc_status.pose.raw.z= pinger_status.z;

			ngc_status.pose.raw.z.valid=true;
			if (pinger_status.device_status!=DEVICE_RUNNING)
			{
				ngc_status.pose.raw.z.valid=false;
			}


			if (ngc_status.pose.raw.z.valid==true) ngc_status.pose.raw.z.timeStamp= pinger_status.timeStamp;

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



#endif /* TASKS_RAWVERPOSFROMUSBL_H_ */
