/*
 * Tasks.h
 *
 *  Created on: Feb 15, 2019
 *      Author: mx
 */

#ifndef TASKS_H_
#define TASKS_H_

#include <vector>
#include "../common/generic/util.h"
#include "../common/generic/custom_types.h"
#include "../common/generic/define.h"
#include "../common/generic/RobotThread.h"
#include "../common/network/NetworkManager.h"
#include "../common/network/CommLink.h"

#include "../common/tasks/RobotTask.h"
#include "Europe_status.h"

#include "../common/tasks/europe/AngFilter.h"
#include "../common/tasks/europe/HorVelFilter.h"
#include "../common/tasks/europe/HorPosFilter.h"
#include "../common/tasks/europe/VerVelFilter.h"
#include "../common/tasks/europe/VerPosFilter.h"
#include "../common/tasks/europe/AltFilter.h"

#include "../common/tasks/RawAngFromFOG.h"
#include "../common/tasks/RawAngFromAHRS.h"
#include "../common/tasks/ActualAngFromRaw.h"
#include "../common/tasks/ActualAngFromFilter.h"

#include "../common/tasks/RawHorVelFromDVL.h"
#include "../common/tasks/RawHorVelFromGPS.h"
#include "../common/tasks/RawHorVelFromUSBL.h"
#include "../common/tasks/ActualHorVelFromRaw.h"
#include "../common/tasks/ActualHorVelFromFilter.h"

#include "../common/tasks/RawHorPosFromGPS.h"
#include "../common/tasks/RawHorPosFromUSBL.h"
#include "../common/tasks/ActualHorPosFromRaw.h"
#include "../common/tasks/ActualHorPosFromFilter.h"

#include "../common/tasks/RawVerVelFromDVL.h"
#include "../common/tasks/RawVerVelFromCTD.h"
#include "../common/tasks/RawVerVelFromPA500.h"
#include "../common/tasks/RawVerVelFromECHO.h"
#include "../common/tasks/RawVerVelFromUSBL.h"
#include "../common/tasks/ActualVerVelFromRaw.h"
#include "../common/tasks/ActualVerVelFromFilter.h"

#include "../common/tasks/RawVerPosFromCTD.h"
#include "../common/tasks/RawVerPosFromUSBL.h"
#include "../common/tasks/ActualVerPosFromRaw.h"
#include "../common/tasks/ActualVerPosFromFilter.h"

#include "../common/tasks/RawAltFromPA500.h"
#include "../common/tasks/RawAltFromECHO.h"
#include "../common/tasks/RawAltFromDVL.h"
#include "../common/tasks/ActualAltFromRaw.h"
#include "../common/tasks/ActualAltFromFilter.h"



/*



#include "../common/tasks/ActualHorPosFromRaw.h"
#include "../tasks/ActualHorPosFromFilter.h"

#include "../tasks/AngVelControl.h"
#include "../tasks/AngPosControl.h"
#include "../tasks/HorVelControl.h"
#include "../tasks/HorPosControl.h"

#include "../tasks/PathFollowing_FA.h"
#include "../tasks/PathFollowing_UA.h"
*/

/*

#include "../../tasks/RawAngFromFOG.h"
#include "../../tasks/RawAngFromAHRS.h"



#include "../../tasks/RawHorVelFromDVL.h"
#include "../../tasks/RawHorVelFromGPS.h"
#include "../../tasks/RawHorVelFromUSBL.h"


#include "../../tasks/RawHorPosFromGPS.h"
#include "../../tasks/RawHorPosFromUSBL.h"


#include "../../tasks/RawVerVelFromDVL.h"
#include "../../tasks/RawVerVelFromCTD.h"
#include "../../tasks/RawVerVelFromPA500.h"
#include "../../tasks/RawVerVelFromECHO.h"
#include "../../tasks/RawVerVelFromUSBL.h"
#include "../../tasks/ActualVerVelFromRaw.h"
#include "../../tasks/ActualVerVelFromFilter.h"
#include "../../tasks/VerVelFilter.h"

#include "../../tasks/RawVerPosFromCTD.h"
#include "../../tasks/RawVerPosFromUSBL.h"
#include "../../tasks/ActualVerPosFromRaw.h"
#include "../../tasks/ActualVerPosFromFilter.h"
#include "../../tasks/VerPosFilter.h"

#include "../../tasks/RawAltFromPA500.h"
#include "../../tasks/RawAltFromECHO.h"
#include "../../tasks/RawAltFromDVL.h"
#include "../../tasks/ActualAltFromRaw.h"
#include "../../tasks/ActualAltFromFilter.h"
#include "../../tasks/AltFilter.h"


#include "../../tasks/VerVelControl.h"
#include "../../tasks/VerPosControl.h"
#include "../../tasks/AltControl.h"



*/



#define num_tasks 100

enum Task_cmd
{
	SET_ANG_SENSOR=4,
	SET_HOR_VEL_SENSOR=5,
	SET_HOR_POS_SENSOR=6,
	SET_VER_VEL_SENSOR=7,
	SET_VER_POS_SENSOR=8,
	SET_ALT_SENSOR=9,

	SET_ANG_RAW_FILTER=10,
	SET_HOR_VEL_RAW_FILTER=11,
	SET_HOR_POS_RAW_FILTER=12,
	SET_VER_VEL_RAW_FILTER=13,
	SET_VER_POS_RAW_FILTER=14,
	SET_ALT_RAW_FILTER=15,

	INIT_ANG_FILTER=16,
	INIT_HOR_VEL_FILTER=17,
	INIT_HOR_POS_FILTER=18,
	INIT_VER_VEL_FILTER=19,
	INIT_VER_POS_FILTER=20,
	INIT_ALT_FILTER=21,

	ANG_VEL_CONTROL=26,
	ANG_POS_CONTROL=27,
	HOR_VEL_CONTROL=28,
	HOR_POS_CONTROL=29,
	VER_VEL_CONTROL=30,
	VER_POS_CONTROL=31,
	ALT_CONTROL=32,

	SET_TASK_PARAM=33,

	PATH_FOLLOWING_FA_CONTROL=34,
	PATH_FOLLOWING_UA_CONTROL = 35,
};

enum SENSORS
{
	SENSOR_FOG=0,
	SENSOR_GPS_AHRS=1,
	SENSOR_DVL=2,
	SENSOR_CTD=3,
	SENSOR_PA500=4,
	SENSOR_ECHOLOGGER=5,
	SENSOR_USBL=6,
};


enum SENSORS_READING_MODE
{
	SENSOR_RAW=0,
	SENSOR_FILTER=1,
};

enum CONTROLLER_MODE
{
	CONTROLLER_OFF=0,
	CONTROLLER_ON=1,
};


struct Task_cmd_packet
{
	int64 cmd_type;
	int64 value;
	int64 task_code;
	int64 param;
};


class Tasks:public RobotThread
{
	private:
		CommLink *cmd=NULL;
		NetworkManager *networkManager;
		

		std::vector<RobotTask*> tasks;
		std::vector<RobotTask*> angSensorsToRaw;
		std::vector<RobotTask*> horVelSensorsToRaw;
		std::vector<RobotTask*> horPosSensorsToRaw;
		std::vector<RobotTask*> verVelSensorsToRaw;
		std::vector<RobotTask*> verPosSensorsToRaw;
		std::vector<RobotTask*> altSensorsToRaw;

		RobotTask* rawAngFromFOG = NULL;
		RobotTask* rawAngFromAHRS = NULL;
		RobotTask* angFilter = NULL;
		RobotTask* actualAngFromRaw = NULL;
		RobotTask* actualAngFromFilter = NULL;
		
		RobotTask* rawHorVelFromDVL = NULL;
		RobotTask* rawHorVelFromGPS = NULL;
		RobotTask* rawHorVelFromUSBL = NULL;
		RobotTask* horVelFilter = NULL;
		RobotTask* actualHorVelFromRaw = NULL;
		RobotTask* actualHorVelFromFilter = NULL;

		RobotTask* rawHorPosFromGPS = NULL;
		RobotTask* rawHorPosFromUSBL = NULL;
		RobotTask* horPosFilter = NULL;
		RobotTask* actualHorPosFromRaw = NULL;
		RobotTask* actualHorPosFromFilter = NULL;

		RobotTask* rawVerVelFromDVL = NULL;
		RobotTask* rawVerVelFromCTD = NULL;
		RobotTask* rawVerVelFromPA500 = NULL;
		RobotTask* rawVerVelFromECHO = NULL;
		RobotTask* rawVerVelFromUSBL = NULL;
		RobotTask* verVelFilter = NULL;
		RobotTask* actualVerVelFromRaw = NULL;
		RobotTask* actualVerVelFromFilter = NULL;

		RobotTask* rawVerPosFromCTD = NULL;
		RobotTask* rawVerPosFromUSBL = NULL;
		RobotTask* actualVerPosFromRaw = NULL;
		RobotTask* actualVerPosFromFilter = NULL;
		RobotTask* verPosFilter = NULL;

		RobotTask* rawAltFromPA500 = NULL;
		RobotTask* rawAltFromECHO = NULL;
		RobotTask* rawAltFromDVL = NULL;
		RobotTask* altFilter = NULL;
		RobotTask* actualAltFromRaw = NULL;
		RobotTask* actualAltFromFilter = NULL;

		/*
		
		
		RobotTask* angVelControl = NULL;
		RobotTask* angPosControl = NULL;
		RobotTask* horVelControl = NULL;
		RobotTask* horPosControl = NULL;

		RobotTask* pathFollowing_FA = NULL;
		RobotTask* pathFollowing_UA = NULL;
		*/
		/*

		
		
		std::vector<RobotTask*> horPosSensorsToRaw;
		std::vector<RobotTask*> verVelSensorsToRaw;
		std::vector<RobotTask*> verPosSensorsToRaw;
		std::vector<RobotTask*> altSensorsToRaw;

		

		
		
		


	

		

		RobotTask *rawAltFromPA500=NULL;
		RobotTask *rawAltFromECHO=NULL;
		RobotTask *rawAltFromDVL=NULL;
		RobotTask *actualAltFromRaw=NULL;
		RobotTask *actualAltFromFilter=NULL;
		RobotTask *altFilter=NULL;


		
		RobotTask *verVelControl=NULL;
		RobotTask *verPosControl=NULL;
		RobotTask *altControl=NULL;

		

		*/


		int taskMap[num_tasks][2];

		Europe_status* status;


	public:
		Tasks(const char *name,NetworkManager &nm, Europe_status* status);
		~Tasks();

		virtual void execute();

		void read_cmd();
		void exec_cmd(std::vector<std::string>& cmd_msg);
};

void* start_tasks(void *arg);

#endif /* TASKS_H_ */
