/*
 * ActualHorVelFromRaw.h
 *
 *  Created on: Feb 8, 2019
 *      Author: mx
 */

#ifndef TASKS_ACTUALHORVELFROMRAW_H_
#define TASKS_ACTUALHORVELFROMRAW_H_


#include "RobotTask.h"



class ActualHorVelFromRaw:public RobotTask
{
	private:
		DataAccess<NGC_status>* ngc_access = NULL;
		DataAccess<Task_status>* task_access = NULL;


	public:
		ActualHorVelFromRaw(const char* name, DataAccess<Task_status>& Task_access, DataAccess<NGC_status>& NGC_access) :RobotTask(name)
		{
			task_access = &Task_access;
			ngc_access = &NGC_access;
		}


		virtual ~ActualHorVelFromRaw(){}


		virtual void execute()
		{
			Task_status task_status;
			task_status= task_access->get();

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
			ngc_status=ngc_access->get();

			ngc_status.velocity_body.actual.u=ngc_status.velocity_body.raw.u;
			ngc_status.velocity_body.actual.v=ngc_status.velocity_body.raw.v;
			ngc_status.velocity_body.actual.u_sea=ngc_status.velocity_body.raw.u_sea;
			ngc_status.velocity_body.actual.v_sea=ngc_status.velocity_body.raw.v_sea;
			ngc_status.velocity_inertial.actual.xDot=ngc_status.velocity_inertial.raw.xDot;
			ngc_status.velocity_inertial.actual.yDot=ngc_status.velocity_inertial.raw.yDot;
			ngc_status.velocity_inertial.actual.xDot_sea=ngc_status.velocity_inertial.raw.xDot_sea;
			ngc_status.velocity_inertial.actual.yDot_sea=ngc_status.velocity_inertial.raw.yDot_sea;
			ngc_status.velocity_inertial.actual.speed=ngc_status.velocity_inertial.raw.speed;
			ngc_status.velocity_inertial.actual.course=ngc_status.velocity_inertial.raw.course;

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


#endif /* TASKS_ACTUALHORVELFROMRAW_H_ */
