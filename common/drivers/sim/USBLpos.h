/*
 * USBLpos.h
 *
 *  Created on: Jun 1, 2018
 *      Author: mx
 */

#ifndef DRIVERS_EUROPE_SIM_USBLPOS_H_
#define DRIVERS_EUROPE_SIM_USBLPOS_H_

#include "DeviceSim.h"
#include "../../../drivers/PingerLink.h"
#include "../../../simulators/europe/SimStatus.h"




class USBLpos:public DeviceSim
{
	private:
		double position_noise_avg,position_noise_std;

		std::default_random_engine random_gen;
		std::normal_distribution<double> *position_noise;

	public:
		USBLpos(const char *name,NetworkManager &nm,SimStatus *s);
		~USBLpos();

		virtual int init();

		virtual void execute();
};

void* start_usblpos(void *arg);

#endif /* DRIVERS_EUROPE_SIM_USBLPOS_H_ */
