/*
 * Europe_telemetry.cpp
 *
 *  Created on: Feb 05, 2020
 *      Author: mx
 */

#include "Europe_telemetry.h"



Europe_telemetry::Europe_telemetry(const char* name, NetworkManager& nm, Europe_status* s):RobotThread(name,SCHED_FIFO, TELEMETRY_THREAD_PRIORITY,start_europe_telemetry)
{
	networkManager = &nm;
	status = s;

	tlm = new CommLink("Telemetry", UDP_PURE);
	tlm->open(networkManager->ROBOT_IP, networkManager->TLM_ROBOT_PORT_OUT,
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
	tSleep.tv_sec=TELEMETRY_TIME_SEC;
	tSleep.tv_nsec= TELEMETRY_TIME_NSEC;

	Connection_status connection_status;

	IO_europe_status io_status;
	FOG_status fog_status;
	GPS_AHRS_status gps_ahrs_status;
	DVL_status dvl_status;
	CTD_status ctd_status;
	PA500_status pa500_status;
	Echologger_status echologger_status;
	Pinger_status pinger_status;

	NGC_status ngc_status;


	Task_status raw_Ang_From_FOG_status;
	Task_status raw_Ang_From_AHRS_status;
	Task_status actual_Ang_From_Raw_status;
	Task_status actual_Ang_From_Filter_status;
	Filter_Ang_status filter_Ang_status;

	Task_status raw_HorVel_From_DVL_status;
	Task_status raw_HorVel_From_GPS_status;
	Task_status raw_HorVel_From_USBL_status;
	Task_status actual_HorVel_From_Raw_status;
	Task_status actual_HorVel_From_Filter_status;
	Filter_HorVel_status filter_HorVel_status;


	while(true)
	{
		connection_status = status->connection_status.get();


		io_status = status->io_status.get();
		fog_status = status->fog_status.get();
		gps_ahrs_status = status->gps_ahrs_status.get();
		dvl_status = status->dvl_status.get();
		ctd_status = status->ctd_status.get();
		pa500_status = status->pa500_status.get();
		echologger_status = status->echologger_status.get();
		pinger_status = status->pinger_status.get();


		ngc_status = status->ngc_status.get();

		raw_Ang_From_FOG_status = status->raw_Ang_From_FOG_status.get();
		raw_Ang_From_AHRS_status = status->raw_Ang_From_AHRS_status.get();
		actual_Ang_From_Raw_status = status->actual_Ang_From_Raw_status.get();
		actual_Ang_From_Filter_status = status->actual_Ang_From_Filter_status.get();
		filter_Ang_status = status->filter_Ang_status.get();

		raw_HorVel_From_DVL_status = status->raw_HorVel_From_DVL_status.get();
		raw_HorVel_From_GPS_status = status->raw_HorVel_From_GPS_status.get();
		raw_HorVel_From_USBL_status = status->raw_HorVel_From_USBL_status.get();
		actual_HorVel_From_Raw_status = status->actual_HorVel_From_Raw_status.get();
		actual_HorVel_From_Filter_status = status->actual_HorVel_From_Filter_status.get();
		filter_HorVel_status = status->filter_HorVel_status.get();

		
		if (connection_status.ethernet_active)
		{
			io_status.compose_tlm_packet(msg.io);
			fog_status.compose_tlm_packet(msg.fog);
			gps_ahrs_status.compose_tlm_packet(msg.gps_ahrs);
			dvl_status.compose_tlm_packet(msg.dvl);
			ctd_status.compose_tlm_packet(msg.ctd);
			pa500_status.compose_tlm_packet(msg.pa500);
			echologger_status.compose_tlm_packet(msg.echologger);
			pinger_status.compose_tlm_packet(msg.pinger);

			ngc_status.compose_tlm_packet(msg.ngc);

			raw_Ang_From_FOG_status.compose_tlm_packet(msg.raw_Ang_From_FOG);
			raw_Ang_From_AHRS_status.compose_tlm_packet(msg.raw_Ang_From_AHRS);
			actual_Ang_From_Raw_status.compose_tlm_packet(msg.actual_Ang_From_Raw);
			actual_Ang_From_Filter_status.compose_tlm_packet(msg.actual_Ang_From_Filter);
			filter_Ang_status.compose_tlm_packet(msg.filter_Ang);

			raw_HorVel_From_DVL_status.compose_tlm_packet(msg.raw_HorVel_From_DVL);
			
			raw_HorVel_From_GPS_status.compose_tlm_packet(msg.raw_HorVel_From_GPS);
			raw_HorVel_From_USBL_status.compose_tlm_packet(msg.raw_HorVel_From_USBL);
			actual_HorVel_From_Raw_status.compose_tlm_packet(msg.actual_HorVel_From_Raw);
			actual_HorVel_From_Filter_status.compose_tlm_packet(msg.actual_HorVel_From_Filter);
			filter_HorVel_status.compose_tlm_packet(msg.filter_HorVel);
			
			tlm->send_message((char*)&msg, sizeof(msg));

		}

		else if (!connection_status.ethernet_active && connection_status.acoustics_active)
		{
			char io_string[BUF_SIZE];
			io_status.compose_string_packet(io_string);

			char fog_string[BUF_SIZE];
			fog_status.compose_string_packet(fog_string);

			char gps_ahrs_string[BUF_SIZE];
			gps_ahrs_status.compose_string_packet(gps_ahrs_string);

			char ctd_string[BUF_SIZE];
			ctd_status.compose_string_packet(ctd_string);

			char dvl_string[BUF_SIZE];
			dvl_status.compose_string_packet(dvl_string);

			char pa500_string[BUF_SIZE];
			pa500_status.compose_string_packet(pa500_string);

			char echologger_string[BUF_SIZE];
			echologger_status.compose_string_packet(echologger_string);

			char pinger_string[BUF_SIZE];
			pinger_status.compose_string_packet(pinger_string);

			char ngc_string[BUF_SIZE];
			ngc_status.compose_string_packet(ngc_string);

			sprintf(tlm_string,"%s %s %s %s %s %s %s %s %s", io_string, fog_string, gps_ahrs_string, ctd_string, dvl_string, pa500_string, echologger_string, pinger_string, ngc_string);
			//printf("tlm: %s\n", tlm_string);

			strcpy( pinger_status.acoustic_tlm , tlm_string );
			status->pinger_status.set(pinger_status);
		}

		nanosleep(&tSleep,NULL);
	}
}



void* start_europe_telemetry(void *arg)
{
	Europe_telemetry* pThread=(Europe_telemetry*)((ThreadInfo*)arg)->pThread;
	pThread->execute();
	return NULL;
}


