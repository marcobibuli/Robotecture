/*
 * ActualAltFromFilter.h
 *
 *  Created on: Feb 8, 2019
 *      Author: mx
 */

#ifndef TASKS_ACTUALALTFROMFILTER_H_
#define TASKS_ACTUALALTFROMFILTER_H_


#include "RobotTask.h"



class ActualAltFromFilter:public RobotTask
{
	private:
		DataAccess<Task_status>* task_access = NULL;
		DataAccess<NGC_status>* ngc_access = NULL;

	public:
		ActualAltFromFilter(const char *name, DataAccess<Task_status>& Task_access, DataAccess<NGC_status>& NGC_access) :RobotTask(name)
		{
			task_access = &Task_access;
			ngc_access = &NGC_access;
		}


		virtual ~ActualAltFromFilter(){}


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
			NGC_status ngc_status;
			ngc_status = ngc_access->get();

			ngc_status.pose.actual.altitude=ngc_status.pose.filtered.altitude;

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



#endif /* TASKS_ACTUALALTFROMFILTER_H_ */
