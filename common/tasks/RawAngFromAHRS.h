/*
 * RawAngFromAHRS.h
 *
 *  Created on: Feb 8, 2019
 *      Author: mx
 */

#ifndef TASKS_RAWANGFROMAHRS_H_
#define TASKS_RAWANGFROMAHRS_H_


#include "RobotTask.h"
#include "../drivers/Device.h"
#include "../data/DataAccess.h"


class RawAngFromAHRS:public RobotTask
{
	private:
		DataAccess<Task_status>* task_access;
		DataAccess<GPS_AHRS_status>* gps_ahrs_access;
		DataAccess<NGC_status>* ngc_access;

	public:
		RawAngFromAHRS(const char *name, DataAccess<Task_status>& Task_access, DataAccess<GPS_AHRS_status>& GPS_AHRS_access, DataAccess<NGC_status>& NGC_access):RobotTask(name)
		{
			task_access = &Task_access;
			gps_ahrs_access = &GPS_AHRS_access;
			ngc_access = &NGC_access;
		}


		virtual ~RawAngFromAHRS(){}

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
			GPS_AHRS_status gps_ahrs_status;
			NGC_status ngc_status;

			gps_ahrs_status= gps_ahrs_access->get();
			ngc_status=ngc_access->get();

			ngc_status.pose.raw.phi=gps_ahrs_status.roll;
			ngc_status.pose.raw.theta=gps_ahrs_status.pitch;
			ngc_status.pose.raw.psi=gps_ahrs_status.heading;

			ngc_status.velocity_body.raw.p=gps_ahrs_status.xGyro;
			ngc_status.velocity_body.raw.q=gps_ahrs_status.yGyro;
			ngc_status.velocity_body.raw.r=gps_ahrs_status.zGyro;

			ngc_status.pose.raw.phi.valid=true;
			ngc_status.pose.raw.theta.valid=true;
			ngc_status.pose.raw.psi.valid=true;
			ngc_status.velocity_body.raw.p.valid=true;
			ngc_status.velocity_body.raw.q.valid=true;
			ngc_status.velocity_body.raw.r.valid=true;
			if (gps_ahrs_status.ahrs_status==DEVICE_OFF)
			{
				ngc_status.pose.raw.phi.valid=false;
				ngc_status.pose.raw.theta.valid=false;
				ngc_status.pose.raw.psi.valid=false;
				ngc_status.velocity_body.raw.p.valid=false;
				ngc_status.velocity_body.raw.q.valid=false;
				ngc_status.velocity_body.raw.r.valid=false;
			}


			if (ngc_status.pose.raw.phi.valid==true) ngc_status.pose.raw.phi.timeStamp=gps_ahrs_status.timeStamp;
			if (ngc_status.pose.raw.theta.valid==true) ngc_status.pose.raw.theta.timeStamp=gps_ahrs_status.timeStamp;
			if (ngc_status.pose.raw.psi.valid==true) ngc_status.pose.raw.psi.timeStamp=gps_ahrs_status.timeStamp;
			if (ngc_status.velocity_body.raw.p.valid==true) ngc_status.velocity_body.raw.p.timeStamp=gps_ahrs_status.timeStamp;
			if (ngc_status.velocity_body.raw.q.valid==true) ngc_status.velocity_body.raw.q.timeStamp=gps_ahrs_status.timeStamp;
			if (ngc_status.velocity_body.raw.r.valid==true) ngc_status.velocity_body.raw.r.timeStamp=gps_ahrs_status.timeStamp;


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


#endif /* TASKS_RAWANGFROMAHRS_H_ */
