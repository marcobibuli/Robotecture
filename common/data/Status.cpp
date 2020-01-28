/*
 * Status.cpp
 *
 *  Created on: Dec 21, 2017
 *      Author: mx
 */

#include "Status.h"



Status::Status(const char *name):RobotThread(name,SCHED_FIFO,STATUS_THREAD_PRIORITY,start_status)
{
	
}


Status::~Status()
{

}



void Status::execute()
{
	Time_status ts;

	tSleep.tv_sec=CORE_SAMPLE_TIME_SEC;
	tSleep.tv_nsec=CORE_SAMPLE_TIME_NSEC;


	clock_gettime(CLOCK_REALTIME,&tStart);

	while(true)
	{
		nanosleep(&tSleep,NULL);

		clock_gettime(CLOCK_REALTIME,&tDelay);
		int64 msec=(tDelay.tv_sec-tStart.tv_sec)*1000 + (tDelay.tv_nsec-tStart.tv_nsec)/1000000;
		
		
		
		struct tm* timeinfo;
		char buffer_time[80];
		char buf[256];

		timeinfo = gmtime(&(tDelay.tv_sec));
		//timeinfo = localtime(&(tDelay.tv_sec));

		strftime(buffer_time, 80, "%F %T", timeinfo);
		sprintf(buf, "%s.%d", buffer_time, tDelay.tv_nsec / 1000000);
		sscanf(buf, "%d-%d-%d %d:%d:%d.%d",&(ts.year), &(ts.month), &(ts.day), &(ts.hour), &(ts.min), &(ts.sec), &(ts.msec));

		//printf("%d-%d-%d %d:%d:%d.%d\n", (ts.year), (ts.month), (ts.day), (ts.hour), (ts.min), (ts.sec), (ts.msec));
		//printf("%u Local time: %s\n", tDelay.tv_sec, buf);
		
		
		ts.timeStamp+=msec;
		ts.dt=((double)msec)/1000.0;
		time_status.set(ts);

		//printf("ts: %lf\n",ts.dt);



		tStart.tv_sec=tDelay.tv_sec;
		tStart.tv_nsec=tDelay.tv_nsec;
	}
}



void* start_status(void *arg)
{
	Status* pThread=(Status*)((ThreadInfo*)arg)->pThread;
	pThread->execute();
	return NULL;
}


