/*
 * NGC.h
 *
 *  Created on: Jan 10, 2018
 *      Author: mx
 */

#ifndef NGC_H_
#define NGC_H_



#include "../common/generic/custom_types.h"
#include "../common/generic/define.h"
#include "../common/generic/RobotThread.h"
#include "../common/generic/RobotMath.h"
#include "../common/network/NetworkManager.h"
#include "../common/network/CommLink.h"
#include "../common/data/NGC_status.h"
#include "../common/data/Time_status.h"
#include "../common/data/DataAccess.h"

 


 class NGC:public RobotThread
 {
 	 private:
	 	 NetworkManager *networkManager;

	 	 CommLink *cmd=NULL;
	 	 

		 DataAccess<Time_status>* time_access = NULL;
		 DataAccess<NGC_status>* ngc_access = NULL;


	 	 double vvfl_prev,vvfr_prev,vvrl_prev,vvrr_prev;
	 	 double vhfl_prev,vhfr_prev,vhrl_prev,vhrr_prev;

 	 public:
	 	 NGC(const char *name,NetworkManager &nm, DataAccess<NGC_status>& NGC_access, DataAccess<Time_status>& Time_access);
	 	 ~NGC();

	 	 virtual void execute();

	 	 void read_cmd();
 };

 void* start_ngc(void *arg);



#endif /* NGC_H_ */
