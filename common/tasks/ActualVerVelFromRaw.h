/*
 * ActualVerVelFromRaw.h
 *
 *  Created on: Feb 8, 2019
 *      Author: mx
 */

#ifndef TASKS_ACTUALVERVELFROMRAW_H_
#define TASKS_ACTUALVERVELFROMRAW_H_

#include "RobotTask.h"

class ActualVerVelFromRaw:public RobotTask
{
	private:
		DataAccess<NGC_status>* ngc_access = NULL;
		DataAccess<Task_status>* task_access = NULL;


	public:
		ActualVerVelFromRaw(const char *name, DataAccess<Task_status>& Task_access, DataAccess<NGC_status>& NGC_access) :RobotTask(name)
		{
			task_access = &Task_access;
			ngc_access = &NGC_access;
		}


		virtual ~ActualVerVelFromRaw(){}


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

			ngc_status.velocity_body.actual.w=ngc_status.velocity_body.raw.w;
			ngc_status.velocity_inertial.actual.zDot=ngc_status.velocity_inertial.raw.zDot;


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


#endif /* TASKS_ACTUALVERVELFROMRAW_H_ */
