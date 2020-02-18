/*
 * IO_europeSim.h
 *
 *  Created on: Feb 13, 2020
 *      Author: mx
 */

#ifndef DRIVERS_IO_EUROPESIM_H_
#define DRIVERS_IO_EUROPESIM_H_


#include <unistd.h>
#include "../DeviceSim.h"
#include "../../europe/IO_europe.h"
#include "../../../../europeSim/EuropeSim.h"
#include "../../../generic/custom_types.h"
#include "../../../network/CommLink.h"

#include "../../../data/IO_europe_status.h"





class IO_europeSim:public DeviceSim
{
	private:
		int digitalInput[EUROPE_DIO_MAX_CHANNELS];
	  	int digitalOutput[EUROPE_DIO_MAX_CHANNELS];
	  	int digitalOutputPulseCounter[EUROPE_DIO_MAX_CHANNELS];
	  	double analogInput[EUROPE_AD_MAX_CHANNELS];
	  	double analogOutput[EUROPE_DA_MAX_CHANNELS];




	public:
		IO_europeSim(const char *name,NetworkManager &nm, SimStatus*s);
		~IO_europeSim();

		virtual int init();

		virtual void execute();

		void read_cmd();
		void send_tlm();

		void setStatus();
};

void* start_io_europeSim(void *arg);

#endif /* DRIVERS_IO_EUROPESIM_H_ */
