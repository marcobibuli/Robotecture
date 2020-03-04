/*
 * ActualHorVelFromFilter.h
 *
 *  Created on: Feb 8, 2019
 *      Author: mx
 */

#ifndef TASKS_ACTUALHORVELFROMFILTER_H_
#define TASKS_ACTUALHORVELFROMFILTER_H_


#include "RobotTask.h"



class ActualHorVelFromFilter:public RobotTask
{
	private:
		DataAccess<NGC_status>* ngc_access = NULL;
		DataAccess<Task_status>* task_access = NULL;

	public:
		ActualHorVelFromFilter(const char* name, DataAccess<Task_status>& Task_access, DataAccess<NGC_status>& NGC_access) :RobotTask(name)
		{
			task_access = &Task_access;
			ngc_access = &NGC_access;
		}


		virtual ~ActualHorVelFromFilter(){}


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

			ngc_status.velocity_body.actual.u=ngc_status.velocity_body.filtered.u;
			ngc_status.velocity_body.actual.v=ngc_status.velocity_body.filtered.v;
			ngc_status.velocity_body.actual.u_sea=ngc_status.velocity_body.filtered.u_sea;
			ngc_status.velocity_body.actual.v_sea=ngc_status.velocity_body.filtered.v_sea;
			ngc_status.velocity_inertial.actual.xDot=ngc_status.velocity_inertial.filtered.xDot;
			ngc_status.velocity_inertial.actual.yDot=ngc_status.velocity_inertial.filtered.yDot;
			ngc_status.velocity_inertial.actual.xDot_sea=ngc_status.velocity_inertial.filtered.xDot_sea;
			ngc_status.velocity_inertial.actual.yDot_sea=ngc_status.velocity_inertial.filtered.yDot_sea;
			ngc_status.velocity_inertial.actual.speed=ngc_status.velocity_inertial.filtered.speed;
			ngc_status.velocity_inertial.actual.course=ngc_status.velocity_inertial.filtered.course;

			ngc_access->set(ngc_status);
		}


		virtual void setStatus(TaskStatus ts)
		{
			Task_status task_status;
			task_status = task_access->get();
			task_status.execution=ts;
			task_access->set(task_status);
		}


		virtual TaskStatus getStatus()
		{
			Task_status task_status;
			task_status = task_access->get();
			return task_status.execution;
		}
};


#endif /* TASKS_ACTUALHORVELFROMFILTER_H_ */
