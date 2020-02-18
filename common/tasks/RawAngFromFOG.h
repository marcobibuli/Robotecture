/*
 * RawAngFromFOG.h
 *
 *  Created on: Feb 8, 2019
 *      Author: mx
 */

#ifndef TASKS_RAWANGFROMFOG_H_
#define TASKS_RAWANGFROMFOG_H_


#include "RobotTask.h"
#include "../drivers/Device.h"
#include "../data/DataAccess.h"


class RawAngFromFOG:public RobotTask
{
	private:
		DataAccess<Task_status>* task_access;
		DataAccess<FOG_status>* fog_access;
		DataAccess<NGC_status>* ngc_access;

	public:
		RawAngFromFOG(const char *name,DataAccess<Task_status>& Task_access, DataAccess<FOG_status>& FOG_access, DataAccess<NGC_status>& NGC_access):RobotTask(name)
		{
			task_access = &Task_access;
			fog_access = &FOG_access;
			ngc_access = &NGC_access;
		}


		virtual ~RawAngFromFOG(){}

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

			FOG_status fog_status;
			NGC_status ngc_status;

			fog_status = fog_access->get();
			ngc_status = ngc_access->get();

			ngc_status.pose.raw.phi=fog_status.roll;
			ngc_status.pose.raw.theta=fog_status.pitch;
			ngc_status.pose.raw.psi=fog_status.heading;

			ngc_status.velocity_body.raw.p=fog_status.rollRate;
			ngc_status.velocity_body.raw.q=fog_status.pitchRate;
			ngc_status.velocity_body.raw.r=fog_status.headingRate;

			ngc_status.pose.raw.phi.valid=true;
			ngc_status.pose.raw.theta.valid=true;
			ngc_status.pose.raw.psi.valid=true;
			ngc_status.velocity_body.raw.p.valid=true;
			ngc_status.velocity_body.raw.q.valid=true;
			ngc_status.velocity_body.raw.r.valid=true;
			if (fog_status.device_status!=DEVICE_RUNNING)
			{
				ngc_status.pose.raw.phi.valid=false;
				ngc_status.pose.raw.theta.valid=false;
				ngc_status.pose.raw.psi.valid=false;
				ngc_status.velocity_body.raw.p.valid=false;
				ngc_status.velocity_body.raw.q.valid=false;
				ngc_status.velocity_body.raw.r.valid=false;
			}


			if (ngc_status.pose.raw.phi.valid==true) ngc_status.pose.raw.phi.timeStamp=fog_status.timeStamp;
			if (ngc_status.pose.raw.theta.valid==true) ngc_status.pose.raw.theta.timeStamp=fog_status.timeStamp;
			if (ngc_status.pose.raw.psi.valid==true) ngc_status.pose.raw.psi.timeStamp=fog_status.timeStamp;
			if (ngc_status.velocity_body.raw.p.valid==true) ngc_status.velocity_body.raw.p.timeStamp=fog_status.timeStamp;
			if (ngc_status.velocity_body.raw.q.valid==true) ngc_status.velocity_body.raw.q.timeStamp=fog_status.timeStamp;
			if (ngc_status.velocity_body.raw.r.valid==true) ngc_status.velocity_body.raw.r.timeStamp=fog_status.timeStamp;


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


#endif /* TASKS_RAWANGFROMFOG_H_ */
