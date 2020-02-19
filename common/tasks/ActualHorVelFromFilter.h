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


	public:
		ActualHorVelFromFilter(const char *name,Status *st):RobotTask(name,st)
		{

		}


		virtual ~ActualHorVelFromFilter(){}


		virtual void execute()
		{
			Task_status task_status;
			task_status=status->actual_HorVel_From_Filter_status.get();

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
			ngc_status=status->ngc_status.get();

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

			status->ngc_status.set(ngc_status);
		}


		virtual void setStatus(TaskStatus ts)
		{
			Task_status task_status;
			task_status=status->actual_HorVel_From_Filter_status.get();
			task_status.execution=ts;
			status->actual_HorVel_From_Filter_status.set(task_status);
		}


		virtual TaskStatus getStatus()
		{
			Task_status task_status;
			task_status=status->actual_HorVel_From_Filter_status.get();
			return task_status.execution;
		}
};


#endif /* TASKS_ACTUALHORVELFROMFILTER_H_ */
