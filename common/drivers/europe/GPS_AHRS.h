/*
 * GPS_AHRS.h
 *
 *  Created on: Jul 24, 2018
 *      Author: mx
 */

#ifndef DRIVERS_GPS_AHRS_H_
#define DRIVERS_GPS_AHRS_H_


#include "../Device.h"
#include "../../generic/custom_types.h"
#include "../../generic/RobotMath.h"
#include "../../network/CommLink.h"
#include "../../data/GPS_AHRS_status.h"
#include "../../data/Time_status.h"
#include "../../data/DataAccess.h"


struct GPS_AHRS_Sim_Data
{
	double x;
	double y;
	double yaw;
	double u;
	double v;
	double r;
};


class GPS_AHRS:public Device
{
	private:
    	double lat0,lon0,x0,y0,utmzone;
    	char utmzone_char;
		DeviceStatus ahrs_status, gps_status;
		CommLink *tlmHMI;

		DataAccess<Time_status> *time_access=NULL;
		DataAccess<GPS_AHRS_status>* gps_ahrs_access = NULL;

	public:
		GPS_AHRS(const char *name,NetworkManager &nm, DataAccess<Time_status>& Time_access, DataAccess<GPS_AHRS_status>& Gps_ahrs_access);
		~GPS_AHRS();

		virtual int init_sim();
		virtual int init_act();

		int init_pos();

		virtual void execute_sim();
		virtual void execute_act();

		void raw_data();

		void update_device_status(int r);

		void read_tlm();
		void read_sim_tlm();


		void send_telemetry();
};


void* start_gps_ahrs(void *arg);


#endif /* DRIVERS_GPS_AHRS_H_ */
