/*
 * RobotThread.h
 *
 *  Created on: Nov 10, 2016
 *      Author: mx
 */

#ifndef COMMON_ROBOTTHREAD_H_
#define COMMON_ROBOTTHREAD_H_

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/signal.h>

#include "define.h"

class RobotThread;
typedef struct
{
	RobotThread* pThread;
	// void* arg;
}ThreadInfo;

class RobotThread
{
	private:
		pthread_t threadId;
		pthread_attr_t threadAttr;
		int thread_policy;
		int thread_priority;
		void*(*threadRoutine)(void*);
		ThreadInfo threadInfo;


	protected:
		char threadName[nameLength];
		bool running;
		bool terminated;


	public:
		RobotThread(const char *name,int policy,int priority,void*(*threadRoutine)(void*));
		virtual ~RobotThread(){}

		virtual void create();
		virtual void terminate();

		const pthread_t* get_id(void) const {return(&threadId);};
		const char* get_name(void) const {return(threadName);};
		const bool isTerminated(void) const {return terminated;}
};


#endif /* COMMON_ROBOTTHREAD_H_ */
