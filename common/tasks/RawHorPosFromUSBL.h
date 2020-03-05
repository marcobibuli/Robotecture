/*
 * RawHorPosFromUSBL.h
 *
 *  Created on: Feb 04, 2019
 *      Author: mx
 */

#ifndef _RAW_HOR_POS_FROM_USBL_H_
#define _RAW_HOR_POS_FROM_USBL_H_

#include "RobotTask.h"
#include "../drivers/Device.h"


class RawHorPosFromUSBL:public RobotTask
{
	private:

		DataAccess<Task_status>* task_access;
		DataAccess<Pinger_status>* pinger_access;
		DataAccess<NGC_status>* ngc_access;

	public:
		RawHorPosFromUSBL(const char *name, DataAccess<Task_status>& Task_access, DataAccess<Pinger_status>& Pinger_access, DataAccess<NGC_status>& NGC_access) :RobotTask(name)
		{
			task_access = &Task_access;
			pinger_access = &Pinger_access;
			ngc_access = &NGC_access;
		}


		virtual ~RawHorPosFromUSBL(){}


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
			Pinger_status pinger_status;
			NGC_status ngc_status;
			
			pinger_status = pinger_access->get();
			ngc_status = ngc_access->get();

			ngc_status.pose.raw.x= pinger_status.x;
			ngc_status.pose.raw.y= pinger_status.y;

			ngc_status.pose.raw.x.valid=true;
			ngc_status.pose.raw.y.valid=true;
			if (pinger_status.device_status!=DEVICE_RUNNING)
			{
				ngc_status.pose.raw.x.valid=false;
				ngc_status.pose.raw.y.valid=false;
			}


			if (ngc_status.pose.raw.x.valid==true) ngc_status.pose.raw.x.timeStamp= pinger_status.timeStamp;
			if (ngc_status.pose.raw.y.valid==true) ngc_status.pose.raw.y.timeStamp= pinger_status.timeStamp;

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


#endif /* _RAW_HOR_POSITION_FROM_USBL_H_ */
