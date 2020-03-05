/*
 * IO_europe.h
 *
 *  Created on: Mar 17, 2017
 *      Author: mx
 */

#ifndef DRIVERS_IO_EUROPE_H_
#define DRIVERS_IO_EUROPE_H_

#include <unistd.h>
#include <string>
#include "../../drivers/Device.h"
#include "../../generic/custom_types.h"
#include "../../generic/util.h"
#include "../../network/CommLink.h"
#include "./hardwareLib/definitions.h"
#include "./hardwareLib/globals.h"
#include "./hardwareLib/e-URoPelib.h"
#include "../../data/IO_europe_status.h"
#include "../../data/DVL_status.h"
#include "../../data/PA500_status.h"
#include "../../data/Echologger_status.h"
#include "../../data/Pinger_status.h"
#include "../../data/Time_status.h"
#include "../../data/DataAccess.h"






class IO_europe :public Device
{
	private:
		int digitalInput[EUROPE_DIO_MAX_CHANNELS];
	  	int digitalOutput[EUROPE_DIO_MAX_CHANNELS];
	  	int digitalOutputPulseCounter[EUROPE_DIO_MAX_CHANNELS];
	  	double analogInput[EUROPE_AD_MAX_CHANNELS];
	  	double analogOutput[EUROPE_DA_MAX_CHANNELS];

	  	//Diamond diamond0,diamond1;

	  	int state;
	  	int tickCount;

	  	void automaticStartStop();

		Diamond diamond0, diamond1;
	  	
		DataAccess<Time_status>* time_access = NULL;
		DataAccess<IO_europe_status>* io_access = NULL;
		DataAccess<DVL_status>* dvl_access = NULL;
		DataAccess<PA500_status>* pa500_access = NULL;
		DataAccess<Echologger_status>* echologger_access = NULL;
		DataAccess<Pinger_status>* pinger_access = NULL;

	public:
		IO_europe(const char *name,NetworkManager &nm, DataAccess<IO_europe_status>& IO_access, DataAccess<DVL_status>& DVL_access, DataAccess<PA500_status>& PA500_access, 
			                                           DataAccess<Echologger_status>& Echologger_access, DataAccess<Pinger_status>& Pinger_access, DataAccess<Time_status>& Time_access);
		~IO_europe();

		virtual int init_sim();
		virtual int init_act();

		virtual void execute_sim();
		virtual void execute_act();

		void updateStatus();

		void exec_cmd(std::vector<std::string>& cmd_msg);
		void read_cmd();
		void send_sim_cmd();
		void read_sim_tlm();
		void send_telemetry();

};

void* start_io_europe(void *arg);

#endif /* DRIVERS_IO_EUROPE_H_ */
