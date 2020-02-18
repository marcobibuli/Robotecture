/*
 * RobotTask.h
 *
 *  Created on: Feb 04, 2019
 *      Author: mx
 */

#ifndef _ROBOTTASK_H_
#define _ROBOTTASK_H_

#include "../generic/define.h"
#include "../data/DataAccess.h"
#include "../data/Task_status.h"
#include "../data/NGC_status.h"
#include "../data/Time_status.h"



class RobotTask
{
	private:


	protected:
		char taskName[nameLength];
		DataAccess<Task_status>* task_access;

	public:
		RobotTask(const char *name)
	    {
			strcpy(taskName,name);
	    }

		virtual ~RobotTask(){}

		virtual void init(){}

		virtual void execute()=0;
		virtual void compute()=0;

		virtual void setStatus(TaskStatus ts)=0;
		virtual TaskStatus getStatus()=0;

		virtual void set_param(int64 param_code,double val){}
};


#endif /* _ROBOTTASK_H_ */
