/*
 * RawHorPosFromGPS.h
 *
 *  Created on: Feb 04, 2019
 *      Author: mx
 */

#ifndef _RAW_HOR_POS_FROM_GPS_H_
#define _RAW_HOR_POS_FROM_GPS_H_

#include "RobotTask.h"
#include "../drivers/Device.h"


class RawHorPosFromGPS:public RobotTask
{
	private:
		DataAccess<Task_status>* task_access;
		DataAccess<GPS_AHRS_status>* gps_ahrs_access;
		DataAccess<NGC_status>* ngc_access;

	public:
		RawHorPosFromGPS(const char *name, DataAccess<Task_status>& Task_access, DataAccess<GPS_AHRS_status>& GPS_AHRS_access, DataAccess<NGC_status>& NGC_access) :RobotTask(name)
		{
			task_access = &Task_access;
			gps_ahrs_access = &GPS_AHRS_access;
			ngc_access = &NGC_access;
		}


		virtual ~RawHorPosFromGPS(){}

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

			gps_ahrs_status = gps_ahrs_access->get();
			ngc_status = ngc_access->get();

			ngc_status.pose.raw.x=gps_ahrs_status.x;
			ngc_status.pose.raw.y=gps_ahrs_status.y;

			ngc_status.pose.raw.x.valid=true;
			ngc_status.pose.raw.y.valid=true;
			if (gps_ahrs_status.gps_status!=DEVICE_RUNNING || gps_ahrs_status.llhPositionValidFlags==0)
			{
				ngc_status.pose.raw.x.valid=false;
				ngc_status.pose.raw.y.valid=false;
			}


			if (ngc_status.pose.raw.x.valid==true) ngc_status.pose.raw.x.timeStamp=gps_ahrs_status.timeStamp;
			if (ngc_status.pose.raw.y.valid==true) ngc_status.pose.raw.y.timeStamp=gps_ahrs_status.timeStamp;


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


#endif /* _RAW_HOR_POS_FROM_GPS_H_ */
