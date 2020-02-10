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

#include "../common/data/GPS_AHRS_status.h"
#include "../common/data/FOG_status.h"


class Europe_status :public Status
{
private:
	

public:
	Europe_status(const char* name):Status(name) {}
	~Europe_status() {}

	DataAccess<GPS_AHRS_status> gps_ahrs_status;
	DataAccess<FOG_status> fog_status;
};





#endif /* EUROPE_STATUS_H_ */

