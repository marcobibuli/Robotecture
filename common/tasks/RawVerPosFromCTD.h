/*
 * RawVerPosFromCTD.h
 *
 *  Created on: Feb 8, 2019
 *      Author: mx
 */

#ifndef TASKS_RAWVERPOSFROMCTD_H_
#define TASKS_RAWVERPOSFROMCTD_H_


#include "RobotTask.h"
#include "../drivers/Device.h"


class RawVerPosFromCTD:public RobotTask
{
	private:

		DataAccess<Task_status>* task_access;
		DataAccess<CTD_status>* ctd_access;
		DataAccess<NGC_status>* ngc_access;

	public:
		RawVerPosFromCTD(const char *name, DataAccess<Task_status>& Task_access, DataAccess<CTD_status>& CTD_access, DataAccess<NGC_status>& NGC_access) :RobotTask(name)
		{
			task_access = &Task_access;
			ctd_access = &CTD_access;
			ngc_access = &NGC_access;
		}


		virtual ~RawVerPosFromCTD(){}

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

			CTD_status ctd_status;
			NGC_status ngc_status;

			ctd_status = ctd_access->get();
			ngc_status = ngc_access->get();

			ngc_status.pose.raw.z=ctd_status.depth;

			ngc_status.pose.raw.z.valid=true;
			if (ctd_status.device_status!=DEVICE_RUNNING)
			{
				ngc_status.pose.raw.z.valid=false;
			}


			if (ngc_status.pose.raw.z.valid==true) ngc_status.pose.raw.z.timeStamp=ctd_status.timeStamp;

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



#endif /* TASKS_RAWVERPOSFROMCTD_H_ */
