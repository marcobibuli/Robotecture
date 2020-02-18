/*
 * NGC.cpp
 *
 *  Created on: Jan 10, 2018
 *      Author: mx
 */

#include "NGC.h"



NGC::NGC(const char *name,NetworkManager &nm, DataAccess<NGC_status>& NGC_access, DataAccess<Time_status>& Time_access):RobotThread(name,SCHED_FIFO,NGC_THREAD_PRIORITY,start_ngc)
{
	networkManager=&nm;
	
	time_access = &Time_access;
	ngc_access = &NGC_access;

	cmd=new CommLink( "NGC_cmd" , OVERRIDE);
	cmd->open( networkManager->ROBOT_IP , networkManager->NGC_ROBOT_HMI_PORT_IN ,
			   networkManager->HMI_IP   , networkManager->NGC_HMI_PORT_OUT );
	cmd->create();


	
}


NGC::~NGC()
{
	if (cmd != NULL)
	{
		cmd->terminate();
		delete cmd;
		cmd = NULL;
	}
}



void NGC::execute()
{
	timespec tSleep;
	tSleep.tv_sec= NGC_SLEEP_SEC;
	tSleep.tv_nsec= NGC_SLEEP_NSEC;


	while(true)
	{
		read_cmd();

		

		nanosleep(&tSleep,NULL);
	}
}


void NGC::read_cmd()
{
	NGC_cmd_packet msg;
	int ret;

	do{
		ret=cmd->recv_message((char*)&msg);
		if (ret>0)
		{
			/*
			switch(msg.cmd_type)
			{
			case SET_FU_REF: status->ngc.force.manual.fu=((double)msg.value)/NGC_factor;
			break;

			case SET_FV_REF: status->ngc.force.manual.fv=((double)msg.value)/NGC_factor;
			break;

			case SET_FW_REF: status->ngc.force.manual.fw=((double)msg.value)/NGC_factor;
			break;

			case SET_TZ_REF: status->ngc.force.manual.tz=((double)msg.value)/NGC_factor;
			break;
			};
			*/
		}

	}while(ret>0);
}






 void* start_ngc(void *arg)
 {
	 NGC* pThread=(NGC*)((ThreadInfo*)arg)->pThread;
	 pThread->execute();
	 return NULL;
 }



