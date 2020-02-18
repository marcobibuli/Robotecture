/*
 * GPS_AHRS_Sim.h
 *
 *  Created on: Jan 28, 2019
 *      Author: mx
 */

#ifndef DRIVERS_GPS_AHRS_SIM_H_
#define DRIVERS_GPS_AHRS_SIM_H_

#include <unistd.h>
#include <math.h>
#include "../DeviceSim.h"
#include "../SimStatus.h"
#include "../../../generic/custom_types.h"
#include "../../../network/CommLink.h"
#include "../../europe/GPS_AHRS.h"
#include "../../../data/GPS_AHRS_status.h"



class GPS_AHRS_Sim:public DeviceSim
{
	private:
		double position_noise_avg,position_noise_std;
		double velocity_noise_avg,velocity_noise_std;
		double orientation_noise_avg,orientation_noise_std;
		double orientationRate_noise_avg,orientationRate_noise_std;
		double acceleration_noise_avg,acceleration_noise_std;
		double lat0,lon0,x0,y0;
		double utmzone;
		char utmzone_char;
		std::default_random_engine random_gen;
		std::normal_distribution<double> *position_noise;
		std::normal_distribution<double> *velocity_noise;
		std::normal_distribution<double> *orientation_noise;
		std::normal_distribution<double> *orientationRate_noise;
		std::normal_distribution<double> *acceleration_noise;

		std::normal_distribution<double> *position_noise_uw;
		std::normal_distribution<double> *velocity_noise_uw;

		const double uw_trh=1.0;

		int64 utcTime = 0;

		GPS_AHRS_status gps_ahrs_status;

	public:
		GPS_AHRS_Sim(const char *name,NetworkManager &nm, SimStatus* s);
		~GPS_AHRS_Sim();

		virtual int init();

		virtual void execute();

		void read_cmd();
		void send_tlm();

};

void* start_gps_ahrs_sim(void *arg);


#endif /* DRIVERS_FOG_SIM_H_ */
