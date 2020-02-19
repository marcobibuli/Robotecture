/*
 * PA500_Sim.h
 *
 *  Created on: Jan 29, 2019
 *      Author: mx
 */

#ifndef DRIVERS_PA500_SIM_H_
#define DRIVERS_PA500_SIM_H_

#include <unistd.h>
#include <math.h>
#include "DeviceSim.h"
#include "../../../common/custom_types.h"
#include "../../../network/CommLink.h"
#include "../../../drivers/PA500.h"
#include "../../../simulators/europe/SimStatus.h"



class PA500_Sim:public DeviceSim
{
	private:
	double range_noise_avg,range_noise_std;
		std::default_random_engine random_gen;
		std::normal_distribution<double> *range_noise;

	public:
		PA500_Sim(const char *name,NetworkManager &nm,SimStatus *s);
		~PA500_Sim();

		virtual int init();

		virtual void execute();

		void read_cmd();
		void send_tlm();

};

void* start_pa500Sim(void *arg);


#endif /* DRIVERS_PA500_SIM_H_ */
