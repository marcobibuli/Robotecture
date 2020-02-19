/*
 * DVL_Sim.h
 *
 *  Created on: Jan 29, 2019
 *      Author: mx
 */

#ifndef DRIVERS_DVL_SIM_H_
#define DRIVERS_DVL_SIM_H_

#include <unistd.h>
#include <math.h>
#include "DeviceSim.h"
#include "../../generic/custom_types.h"
#include "../../network/CommLink.h"
#include "../DVL.h"
#include "../../data/DVL_status.h"
#include "../../../europeSim/EuropeSim.h"



class DVL_Sim:public DeviceSim
{
	private:
	    double range_noise_avg,range_noise_std;
		double velocity_noise_avg,velocity_noise_std;

		std::default_random_engine random_gen;
		std::normal_distribution<double> *range_noise;
		std::normal_distribution<double> *velocity_noise;

	public:
		DVL_Sim(const char *name,NetworkManager &nm,SimStatus *s);
		~DVL_Sim();

		virtual int init();

		virtual void execute();

		void read_cmd();
		void send_tlm();

};

void* start_dvlSim(void *arg);


#endif /* DRIVERS_DVL_SIM_H_ */
