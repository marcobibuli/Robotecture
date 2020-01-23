/*
 * RobotThread.cpp
 *
 *  Created on: Nov 10, 2016
 *      Author: mx
 */

#include "RobotThread.h"


RobotThread::RobotThread(const char *name,int policy,int priority,void*(*routine)(void*))
{
	threadId=0;
	strcpy(threadName,name);
	thread_policy=policy;
	thread_priority=priority;
	threadRoutine=routine;
	threadInfo.pThread=this;

	running=true;
	terminated=false;
}


void RobotThread::create()
{
	threadId=0;

	int status;

	if((thread_priority<sched_get_priority_min(thread_policy)) || (thread_priority>sched_get_priority_max(thread_policy)))
	{
	    printf("RobotThread::create - Wrong priority (%d) [min: %d, max: %d]\n",
	    		thread_priority,sched_get_priority_min(thread_policy),
	              sched_get_priority_max(thread_policy));
	    exit(-1);
	}

	status=pthread_attr_init(&threadAttr);
	if (status!=0)
	{
		printf("RobotThread::create - Attr init error\n");
		exit(-1);
	}

	status=pthread_attr_setschedpolicy(&threadAttr,thread_policy);
	if (status!=0)
	{
		printf("RobotThread::create - set sched policy error\n");
		exit(-1);
	}

	struct sched_param threadParam;
	threadParam.sched_priority=thread_priority;
	status=pthread_attr_setschedparam(&threadAttr,&threadParam);
	if (status!=0)
	{
		printf("RobotThread::create - set sched param error\n");
		exit(-1);
	}

	status=pthread_attr_setinheritsched(&threadAttr,PTHREAD_EXPLICIT_SCHED);
	if (status!=0)
	{
		printf("RobotThread::create - set inherit sched error\n");
		exit(-1);
	}

	status=pthread_create(&threadId,&threadAttr,threadRoutine,(void*)(&threadInfo));
	if (status!=0)
	{
		printf("RobotThread::create - thread create error\n");
		exit(-1);
	}

	//printf("Started thread %s %d\n",threadName,threadId);

}


void RobotThread::terminate(void)
{
	/*
	int status;

	//printf("Terminating thread %s %d\n",threadName,threadId);
	status=pthread_cancel(threadId);
	if (status!=0)
	{
		printf("RobotThread::terminate - thread cancel error %d\n",status);
		exit(-1);
	}

	status=pthread_join(threadId,NULL);
	if (status!=0)
	{
		printf("RobotThread::terminate - thread join error\n");
		exit(-1);
	}
	*/

	running=false;
}
