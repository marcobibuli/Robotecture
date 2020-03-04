/*
 * Status.h
 *
 *  Created on: Dec 20, 2017
 *      Author: mx
 */

#ifndef STATUS_H_
#define STATUS_H_


#include "../generic/custom_types.h"
#include "../generic/RobotThread.h"

#include "DataAccess.h"
#include "Time_status.h"
#include "Connection_status.h"
#include <time.h>




class Status:public RobotThread
{
	protected:
		timespec tDelay,tStart,tSleep;

	public:
		Status(const char *name);
		~Status();

		virtual void execute();

		DataAccess<Connection_status> connection_status;
		DataAccess<Time_status> time_status;
};

void* start_status(void *arg);



#endif /* STATUS_H_ */
