/*
 * europe_commands.cpp
 *
 *  Created on: Feb 05, 2020
 *      Author: mx
 */

#include "Europe_commands.h"



Europe_commands::Europe_commands(const char *name):RobotThread(name,SCHED_FIFO,STATUS_THREAD_PRIORITY,start_europe_commands)
{
	
}


Europe_commands::~Europe_commands()
{

}



void Europe_commands::execute()
{
	tSleep.tv_sec=CORE_SAMPLE_TIME_SEC;
	tSleep.tv_nsec=CORE_SAMPLE_TIME_NSEC;

	while(true)
	{

		nanosleep(&tSleep,NULL);
	}
}



void* start_europe_commands(void *arg)
{
	Europe_commands* pThread=(Europe_commands*)((ThreadInfo*)arg)->pThread;
	pThread->execute();
	return NULL;
}


