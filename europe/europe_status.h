/*
 * Europe_status.h
 *
 *  Created on: Jan 24, 2020
 *      Author: mx
 */

#ifndef EUROPE_STATUS_H_
#define EUROPE_STATUS_H_




#include "../common/data/DataAccess.h"
#include "../common/data/Status.h"


#include "../common/data/IO_europe_status.h"
#include "../common/data/FOG_status.h"
#include "../common/data/GPS_AHRS_status.h"
#include "../common/data/DVL_status.h"
#include "../common/data/CTD_status.h"
#include "../common/data/PA500_status.h"
#include "../common/data/Echologger_status.h"
#include "../common/data/Pinger_status.h"

#include "../common/data/NGC_status.h"
#include "../common/data/Task_status.h"

#include "../common/data/Filter_Ang_status.h"
#include "../common/data/Filter_HorVel_status.h"


class Europe_status :public Status
{
	private:
	

	public:
		Europe_status(const char* name):Status(name) {}
		~Europe_status() {}


		DataAccess<IO_europe_status> io_status;
		DataAccess<FOG_status> fog_status;
		DataAccess<GPS_AHRS_status> gps_ahrs_status;
		DataAccess<DVL_status> dvl_status;
		DataAccess<CTD_status> ctd_status;
		DataAccess<PA500_status> pa500_status;
		DataAccess<Echologger_status> echologger_status;
		DataAccess<Pinger_status> pinger_status;
		DataAccess<NGC_status> ngc_status;
	
		DataAccess<Task_status> raw_Ang_From_FOG_status;
		DataAccess<Task_status> raw_Ang_From_AHRS_status;
		DataAccess<Task_status> actual_Ang_From_Raw_status;
		DataAccess<Task_status> actual_Ang_From_Filter_status;
		DataAccess<Filter_Ang_status> filter_Ang_status;

		DataAccess<Task_status> raw_HorVel_From_DVL_status;
		DataAccess<Task_status> raw_HorVel_From_GPS_status;
		DataAccess<Task_status> raw_HorVel_From_USBL_status;
		DataAccess<Task_status> actual_HorVel_From_Raw_status;
		DataAccess<Task_status> actual_HorVel_From_Filter_status;
		DataAccess<Filter_HorVel_status> filter_HorVel_status;

};





#endif /* EUROPE_STATUS_H_ */

