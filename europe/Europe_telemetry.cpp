/*
 * Europe_telemetry.cpp
 *
 *  Created on: Feb 05, 2020
 *      Author: mx
 */

#include "Europe_telemetry.h"



Europe_telemetry::Europe_telemetry(const char* name, NetworkManager& nm, Europe_status* s):RobotThread(name,SCHED_FIFO,STATUS_THREAD_PRIORITY,start_europe_telemetry)
{
	networkManager = &nm;
	status = s;

	tlm = new CommLink("Telemetry", UDP_PURE);
	int x=tlm->open(networkManager->ROBOT_IP, networkManager->TLM_ROBOT_PORT_OUT,
		            networkManager->HMI_IP,   networkManager->TLM_HMI_PORT_IN);
	tlm->create();
}


Europe_telemetry::~Europe_telemetry()
{
	tlm->terminate();
	delete tlm;
}



void Europe_telemetry::execute()
{
	timespec tSleep;
	tSleep.tv_sec=CORE_SAMPLE_TIME_SEC;
	tSleep.tv_nsec=CORE_SAMPLE_TIME_NSEC;


	FOG_status fog_status;
	GPS_AHRS_status gps_ahrs_status;


	while(true)
	{
		fog_status = status->fog_status.get();
		gps_ahrs_status = status->gps_ahrs_status.get();


		fog_status.compose_tlm_packet(msg.fog);
		gps_ahrs_status.compose_tlm_packet(msg.gps_ahrs);


		tlm->send_message((char*)&msg, sizeof(msg));

		nanosleep(&tSleep,NULL);
	}
}



void* start_europe_telemetry(void *arg)
{
	Europe_telemetry* pThread=(Europe_telemetry*)((ThreadInfo*)arg)->pThread;
	pThread->execute();
	return NULL;
}


