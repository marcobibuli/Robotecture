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

#include "../common/data/GPS_AHRS_status.h"





struct TLM_packet
{
	FOG_tlm_packet fog;
	GPS_AHRS_tlm_packet gps_ahrs;

};



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

