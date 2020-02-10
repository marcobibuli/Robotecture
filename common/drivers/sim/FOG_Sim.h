/*
 * FOG_Sim.h
 *
 *  Created on: Jun 8, 2018
 *      Author: mx
 */

#ifndef DRIVERS_FOG_SIM_H_
#define DRIVERS_FOG_SIM_H_

#include <unistd.h>
#include <math.h>
#include "DeviceSim.h"
#include "SimStatus.h"
#include "../../generic/custom_types.h"
#include "../../network/CommLink.h"
#include "../FOG.h"
#include "../../data/FOG_status.h"




class FOG_Sim:public DeviceSim
{
	private:
		double orientation_noise_avg, orientation_noise_std;
		double orientationRate_noise_avg, orientationRate_noise_std;

		std::normal_distribution<double>* orientation_noise;
		std::normal_distribution<double>* orientationRate_noise;

		FOG_status fog_status;

	public:
	  	FOG_Sim(const char *name,NetworkManager &nm,SimStatus *s);
		~FOG_Sim();

		virtual int init();

		virtual void execute();

		void read_cmd();
		void send_tlm();

};

void* start_fogSim(void *arg);


#endif /* DRIVERS_FOG_SIM_H_ */
