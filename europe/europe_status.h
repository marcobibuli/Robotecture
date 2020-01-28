/*
 * europe_status.h
 *
 *  Created on: Jan 24, 2020
 *      Author: mx
 */

#ifndef EUROPE_STATUS_H_
#define EUROPE_STATUS_H_




#include "../common/data/DataAccess.h"
#include "../common/data/Status.h"



class europe_status :public Status
{
private:
	

public:
	europe_status(const char* name):Status(name) {}
	~europe_status() {}

	
};





#endif /* EUROPE_STATUS_H_ */

