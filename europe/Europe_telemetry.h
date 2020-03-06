/*
 * Europe_telemetry.h
 *
 *  Created on: Feb 05, 2020
 *      Author: mx
 */

#ifndef EUROPE_TELEMETRY_H_
#define EUROPE_TELEMETRY_H_



#include "Europe_status.h"

#include "../common/generic/RobotThread.h"
#include "../common/network/NetworkManager.h"
#include "../common/network/CommLink.h"

#include "../common/data/FOG_status.h"
#include "../common/data/GPS_AHRS_status.h"
#include "../common/data/DVL_status.h"
#include "../common/data/CTD_status.h"
#include "../common/data/PA500_status.h"
#include "../common/data/Echologger_status.h"
#include "../common/data/Pinger_status.h"

#include "../common/data/NGC_status.h"


# pragma pack (push, 1)
struct TLM_packet
{
	IO_europe_tlm_packet io;
	FOG_tlm_packet fog;
	GPS_AHRS_tlm_packet gps_ahrs;
	DVL_tlm_packet dvl;
	CTD_tlm_packet ctd;
	PA500_tlm_packet pa500;
	Echologger_tlm_packet echologger;
	Pinger_tlm_packet pinger;

	NGC_tlm_packet ngc;

	Task_tlm_packet raw_Ang_From_FOG;
	Task_tlm_packet raw_Ang_From_AHRS;
	Task_tlm_packet actual_Ang_From_Raw;
	Task_tlm_packet actual_Ang_From_Filter;
	Filter_Ang_tlm_packet filter_Ang;

	Task_tlm_packet raw_HorVel_From_DVL;
	Task_tlm_packet raw_HorVel_From_GPS;
	Task_tlm_packet raw_HorVel_From_USBL;
	Task_tlm_packet actual_HorVel_From_Raw;
	Task_tlm_packet actual_HorVel_From_Filter;
	Filter_HorVel_tlm_packet filter_HorVel;

	Task_tlm_packet raw_HorPos_From_GPS;
	Task_tlm_packet raw_HorPos_From_USBL;
	Task_tlm_packet actual_HorPos_From_Raw;
	Task_tlm_packet actual_HorPos_From_Filter;
	Filter_HorPos_tlm_packet filter_HorPos;
	
	Task_tlm_packet raw_VerVel_From_DVL;
	Task_tlm_packet raw_VerVel_From_CTD;
	Task_tlm_packet raw_VerVel_From_PA500;
	Task_tlm_packet raw_VerVel_From_ECHO;
	Task_tlm_packet raw_VerVel_From_USBL;
	Task_tlm_packet actual_VerVel_From_Raw;
	Task_tlm_packet actual_VerVel_From_Filter;
	Filter_VerVel_tlm_packet filter_VerVel;

	Task_tlm_packet raw_VerPos_From_CTD;
	Task_tlm_packet raw_VerPos_From_USBL;
	Task_tlm_packet actual_VerPos_From_Raw;
	Task_tlm_packet actual_VerPos_From_Filter;
	Filter_VerPos_tlm_packet filter_VerPos;

	Task_tlm_packet raw_Alt_From_PA500;
	Task_tlm_packet raw_Alt_From_ECHO;
	Task_tlm_packet raw_Alt_From_DVL;
	Task_tlm_packet actual_Alt_From_Raw;
	Task_tlm_packet actual_Alt_From_Filter;
	Filter_Alt_tlm_packet filter_Alt;
	
};
# pragma pack (pop)


class Europe_telemetry :public RobotThread
{
	private:
		Europe_status* status;
		NetworkManager* networkManager;
		CommLink* tlm;
		TLM_packet msg;
		char tlm_string[BUF_SIZE];

	public:
		Europe_telemetry(const char* name, NetworkManager& nm, Europe_status* s);
		~Europe_telemetry();

		virtual void execute();

		
};

void* start_europe_telemetry(void* arg);





#endif /* EUROPE_TELEMETRY_H_ */

