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

	IO_europe_status io_status;
	FOG_status fog_status;
	GPS_AHRS_status gps_ahrs_status;
	DVL_status dvl_status;

	NGC_status ngc_status;


	Task_status raw_Ang_From_FOG_status;
	Task_status raw_Ang_From_AHRS_status;
	Task_status actual_Ang_From_Raw_status;
	Task_status actual_Ang_From_Filter_status;

	Filter_Ang_status filter_Ang_status;


	while(true)
	{
		io_status = status->io_status.get();
		fog_status = status->fog_status.get();
		gps_ahrs_status = status->gps_ahrs_status.get();
		dvl_status = status->dvl_status.get();
		ngc_status = status->ngc_status.get();
		raw_Ang_From_FOG_status = status->raw_Ang_From_FOG_status.get();
		raw_Ang_From_AHRS_status = status->raw_Ang_From_AHRS_status.get();
		actual_Ang_From_Raw_status = status->actual_Ang_From_Raw_status.get();
		actual_Ang_From_Filter_status = status->actual_Ang_From_Filter_status.get();
		filter_Ang_status = status->filter_Ang_status.get();

		
		io_status.compose_tlm_packet(msg.io);
		fog_status.compose_tlm_packet(msg.fog);
		gps_ahrs_status.compose_tlm_packet(msg.gps_ahrs);
		dvl_status.compose_tlm_packet(msg.dvl);
		ngc_status.compose_tlm_packet(msg.ngc);
		raw_Ang_From_FOG_status.compose_tlm_packet(msg.raw_Ang_From_FOG);
		raw_Ang_From_AHRS_status.compose_tlm_packet(msg.raw_Ang_From_AHRS);
		actual_Ang_From_Raw_status.compose_tlm_packet(msg.actual_Ang_From_Raw);
		actual_Ang_From_Filter_status.compose_tlm_packet(msg.actual_Ang_From_Filter);
		filter_Ang_status.compose_tlm_packet(msg.filter_Ang);


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


