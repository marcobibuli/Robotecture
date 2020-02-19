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

#include "../common/data/NGC_status.h"


# pragma pack (push, 1)
struct TLM_packet
{
	IO_europe_tlm_packet io;
	FOG_tlm_packet fog;
	GPS_AHRS_tlm_packet gps_ahrs;
	DVL_tlm_packet dvl;

	NGC_tlm_packet ngc;

	Task_tlm_packet raw_Ang_From_FOG;
	Task_tlm_packet raw_Ang_From_AHRS;
	Task_tlm_packet actual_Ang_From_Raw;
	Task_tlm_packet actual_Ang_From_Filter;

	Filter_Ang_tlm_packet filter_Ang;
};
# pragma pack (pop)


class Europe_telemetry :public RobotThread
{
	private:
		Europe_status* status;
		NetworkManager* networkManager;
		CommLink* tlm;
		TLM_packet msg;

	public:
		Europe_telemetry(const char* name, NetworkManager& nm, Europe_status* s);
		~Europe_telemetry();

		virtual void execute();

		
};

void* start_europe_telemetry(void* arg);





#endif /* EUROPE_TELEMETRY_H_ */

