/*
 * CTD_Sim.h
 *
 *  Created on: Jan 28, 2019
 *      Author: mx
 */

#ifndef DRIVERS_CTD_SIM_H_
#define DRIVERS_CTD_SIM_H_

#include <unistd.h>
#include <math.h>
#include "DeviceSim.h"
#include "../../generic/custom_types.h"
#include "../../network/CommLink.h"
#include "../CTD.h"
#include "../../data/CTD_status.h"



class CTD_Sim:public DeviceSim
{
	private:
	    double conductivity_noise_avg,conductivity_noise_std;
		double temperature_noise_avg,temperature_noise_std;
		double depth_noise_avg,depth_noise_std;
		std::default_random_engine random_gen;
		std::normal_distribution<double> *conductivity_noise;
		std::normal_distribution<double> *temperature_noise;
		std::normal_distribution<double> *depth_noise;

	public:
		CTD_Sim(const char *name,NetworkManager &nm,SimStatus *s);
		~CTD_Sim();

		virtual int init();

		virtual void execute();

		void read_cmd();
		void send_tlm();

};

void* start_ctdSim(void *arg);


#endif /* DRIVERS_CTD_SIM_H_ */
