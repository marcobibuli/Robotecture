/*
 * Europe_commands.h
 *
 *  Created on: Feb 05, 2020
 *      Author: mx
 */

#ifndef EUROPE_COMMANDS_H_
#define EUROPE_COMMANDS_H_




#include "../common/generic/RobotThread.h"
#include "Europe_status.h"



class Europe_commands :public RobotThread
{
protected:
	timespec tDelay, tStart, tSleep;

public:
	Europe_commands(const char* name);
	~Europe_commands();

	virtual void execute();

	Europe_status *status;
};

void* start_europe_commands(void* arg);





#endif /* EUROPE_COMMANDS_H_ */