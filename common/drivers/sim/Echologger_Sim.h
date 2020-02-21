/*
 * Echologger_Sim.h
 *
 *  Created on: Feb 05, 2019
 *      Author: mx
 */

#ifndef DRIVERS_ECHOLOGGER_SIM_H_
#define DRIVERS_ECHOLOGGER_SIM_H_

#include <unistd.h>
#include <math.h>
#include "DeviceSim.h"
#include "../Device.h"
#include "../../generic/custom_types.h"
#include "../../network/CommLink.h"
#include "../../data/Echologger_status.h"
#include "../../../europeSim/EuropeSim.h"



class Echologger_Sim:public DeviceSim
{
	private:
	double range_noise_avg,range_noise_std;
		std::default_random_engine random_gen;
		std::normal_distribution<double> *range_noise;

	public:
		Echologger_Sim(const char *name,NetworkManager &nm,SimStatus *s);
		~Echologger_Sim();

		virtual int init();

		virtual void execute();

		void read_cmd();
		void send_tlm();

};

void* start_echologgerSim(void *arg);


#endif /* DRIVERS_ECHOLOGGER_SIM_H_ */
