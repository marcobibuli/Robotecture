/*
 * ActualVerVelFromFilter.h
 *
 *  Created on: Feb 8, 2019
 *      Author: mx
 */

#ifndef TASKS_ACTUALVERVELFROMFILTER_H_
#define TASKS_ACTUALVERVELFROMFILTER_H_


#include "RobotTask.h"



class ActualVerVelFromFilter:public RobotTask
{
	private:
		DataAccess<NGC_status>* ngc_access = NULL;
		DataAccess<Task_status>* task_access = NULL;


	public:
		ActualVerVelFromFilter(const char *name, DataAccess<Task_status>& Task_access, DataAccess<NGC_status>& NGC_access) :RobotTask(name)
		{
			task_access = &Task_access;
			ngc_access = &NGC_access;
		}


		virtual ~ActualVerVelFromFilter(){}


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

			ngc_status.velocity_body.actual.w=ngc_status.velocity_body.filtered.w;
			ngc_status.velocity_inertial.actual.zDot=ngc_status.velocity_inertial.filtered.zDot;

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


#endif /* TASKS_ACTUALVERVELFROMFILTER_H_ */
