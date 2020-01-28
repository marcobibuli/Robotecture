/*
 * NetworkManager.cpp
 *
 *  Created on: Nov 9, 2016
 *      Author: mx
 */

#include "NetworkManager.h"

NetworkManager::NetworkManager()
{
	HEARTHBEAT_ROBOT_PORT_IN=0;
	HEARTHBEAT_HMI_PORT_IN=0;
	HEARTHBEAT_ROBOT_PORT_OUT=0;
	HEARTHBEAT_HMI_PORT_OUT=0;

	IO_CHANNELS_ROBOT_PORT_IN=0;
	IO_CHANNELS_HMI_PORT_IN=0;
	IO_CHANNELS_ROBOT_PORT_OUT=0;
	IO_CHANNELS_HMI_PORT_OUT=0;
	IO_CHANNELS_SIM_PORT_IN=0;
	IO_CHANNELS_SIM_PORT_OUT=0;
	IO_CHANNELS_SIM_ROBOT_PORT_IN=0;
	IO_CHANNELS_SIM_ROBOT_PORT_OUT=0;

	MOTORS_ROBOT_PORT_IN=0;
	MOTORS_HMI_PORT_IN=0;
	MOTORS_ROBOT_PORT_OUT=0;
	MOTORS_HMI_PORT_OUT=0;
	MOTORS_SIM_PORT_IN=0;
	MOTORS_SIM_PORT_OUT=0;
	MOTORS_SIM_ROBOT_PORT_IN=0;
	MOTORS_SIM_ROBOT_PORT_OUT=0;

	ACTUATOR_PORT_BOW_CMD_SIM_PORT_IN=0;
	ACTUATOR_PORT_STERN_CMD_SIM_PORT_IN=0;
	ACTUATOR_STBD_BOW_CMD_SIM_PORT_IN=0;
	ACTUATOR_STBD_STERN_CMD_SIM_PORT_IN=0;
	ACTUATOR_PORT_BOW_TLM_SIM_PORT_OUT=0;
	ACTUATOR_PORT_STERN_TLM_SIM_PORT_OUT=0;
	ACTUATOR_STBD_BOW_TLM_SIM_PORT_OUT=0;
	ACTUATOR_STBD_STERN_TLM_SIM_PORT_OUT=0;

	ACTUATOR_CMD_PORT_IN = 0;
	ACTUATOR_TLM_PORT_OUT = 0;
	ACTUATOR_PB_CMD_ROBOT_PORT_OUT= 0;
	ACTUATOR_PS_CMD_ROBOT_PORT_OUT= 0;
	ACTUATOR_SB_CMD_ROBOT_PORT_OUT= 0;
	ACTUATOR_SS_CMD_ROBOT_PORT_OUT= 0;
	ACTUATOR_PB_TLM_ROBOT_PORT_IN= 0;
	ACTUATOR_PS_TLM_ROBOT_PORT_IN= 0;
	ACTUATOR_SB_TLM_ROBOT_PORT_IN= 0;
	ACTUATOR_SS_TLM_ROBOT_PORT_IN= 0;
	ACTUATOR_PB_TLM_ROBOT_PORT_OUT = 0;
	ACTUATOR_PS_TLM_ROBOT_PORT_OUT = 0;
	ACTUATOR_SB_TLM_ROBOT_PORT_OUT = 0;
	ACTUATOR_SS_TLM_ROBOT_PORT_OUT = 0;

	ACTUATOR_POW_EN_ROBOT_IN = 0;
	ACTUATOR_POW_EN_HMI_OUT = 0;

	GPS_AHRS_ROBOT_PORT_IN=0;
	GPS_AHRS_HMI_PORT_IN=0;
	GPS_AHRS_ROBOT_PORT_OUT=0;
	GPS_AHRS_HMI_PORT_OUT=0;
	GPS_AHRS_SIM_PORT_IN=0;
	GPS_AHRS_SIM_PORT_OUT=0;
	GPS_AHRS_SIM_ROBOT_PORT_IN=0;
	GPS_AHRS_SIM_ROBOT_PORT_OUT=0;
	GPS_AHRS_PORT_IN = 0;
	GPS_AHRS_PORT_OUT = 0;
	GPS_AHRS_ROBOT_HMI_PORT_OUT = 0;

	FOG_ROBOT_PORT_IN=0;
	FOG_HMI_PORT_IN=0;
	FOG_ROBOT_PORT_OUT=0;
	FOG_HMI_PORT_OUT=0;
	FOG_SIM_PORT_IN=0;
	FOG_SIM_PORT_OUT=0;
	FOG_SIM_ROBOT_PORT_IN=0;
	FOG_SIM_ROBOT_PORT_OUT=0;

	DVL_ROBOT_PORT_IN=0;
	DVL_HMI_PORT_IN=0;
	DVL_ROBOT_PORT_OUT=0;
	DVL_HMI_PORT_OUT=0;
	DVL_SIM_PORT_IN=0;
	DVL_SIM_PORT_OUT=0;
	DVL_SIM_ROBOT_PORT_IN=0;
	DVL_SIM_ROBOT_PORT_OUT=0;

	CTD_ROBOT_PORT_IN=0;
	CTD_HMI_PORT_IN=0;
	CTD_ROBOT_PORT_OUT=0;
	CTD_HMI_PORT_OUT=0;
	CTD_SIM_PORT_IN=0;
	CTD_SIM_PORT_OUT=0;
	CTD_SIM_ROBOT_PORT_IN=0;
	CTD_SIM_ROBOT_PORT_OUT=0;

	PA500_ROBOT_PORT_IN=0;
	PA500_HMI_PORT_IN=0;
	PA500_ROBOT_PORT_OUT=0;
	PA500_HMI_PORT_OUT=0;
	PA500_SIM_PORT_IN=0;
	PA500_SIM_PORT_OUT=0;
	PA500_SIM_ROBOT_PORT_IN=0;
	PA500_SIM_ROBOT_PORT_OUT=0;

	ECHOLOGGER_ROBOT_PORT_IN=0;
	ECHOLOGGER_PORT_OUT=0;

	PINGER_ROBOT_PORT_IN=0;
	PINGER_HMI_PORT_IN=0;
	PINGER_ROBOT_PORT_OUT=0;
	PINGER_HMI_PORT_OUT=0;

	NGC_ROBOT_PORT_IN=0;
	NGC_HMI_PORT_IN=0;
	NGC_ROBOT_PORT_OUT=0;
	NGC_HMI_PORT_OUT=0;
	NGC_TO_IO_PORT_OUT=0;

	TASKS_ROBOT_PORT_IN=0;
	TASKS_HMI_PORT_IN=0;
	TASKS_ROBOT_PORT_OUT=0;
	TASKS_HMI_PORT_OUT=0;

	PATHPLANNER_ROBOT_PORT_IN=0;
	PATHPLANNER_HMI_PORT_IN=0;
	PATHPLANNER_ROBOT_PORT_OUT=0;
	PATHPLANNER_HMI_PORT_OUT=0;

	CMD_ROBOT_PORT_IN=0;
	CMD_ROBOT_PORT_OUT=0;
	CMD_SIM_PORT_IN=0;
	CMD_SIM_PORT_OUT=0;
	TLM_ROBOT_PORT_IN=0;
	TLM_ROBOT_PORT_OUT=0;
	TLM_SIM_PORT_IN=0;
	TLM_SIM_PORT_OUT=0;

	PINGER_SIM_PORT_IN=0;
	PINGER_SIM_PORT_OUT=0;
	USBL_SIM_PORT_IN=0;
	USBL_SIM_PORT_OUT=0;
	USBL_POS_SIM_PORT_OUT=0;
	USBL_POS_PORT_IN=0;
	USBL_POS_PORT_OUT=0;
	USBL_TLM_PORT_OUT=0;
	USBL_HMI_TLM_PORT_IN=0;
	USBL_CMD_PORT_IN=0;
	USBL_HMI_CMD_PORT_OUT=0;

	CORE_ROBOT_PORT_IN=0.0;
	CORE_ROBOT_PORT_OUT=0.0;
	CORE_DRIVER_PORT_IN=0.0;
	CORE_DRIVER_PORT_OUT=0.0;
}


NetworkManager::~NetworkManager()
{

}

int NetworkManager::init()
{
	char name_dir_file[256];

	sprintf(CONFIGURATION_FILES_DIRECTORY,"../../../%s/config/",ROBOT_NAME);

	sprintf(name_dir_file,"%s%s",CONFIGURATION_FILES_DIRECTORY,CONFIGURATION_NETWORK_FILE);

	return init(name_dir_file);
}


int NetworkManager::init(const char *name_dir_file)
{
	FILE *f;
	char var[100],value[100];


	if((f=fopen(name_dir_file,"r"))!=NULL)
	{

		while(!feof(f))
		{
			fscanf(f,"%s %s\n",var,value);


			if (strcmp(var,"ROBOT_IP")==0) strcpy(ROBOT_IP,value);
			if (strcmp(var,"HMI_IP")==0) strcpy(HMI_IP,value);
			if (strcmp(var,"SIM_IP")==0) strcpy(SIM_IP,value);
			if (strcmp(var,"USBL_IP")==0) strcpy(USBL_IP,value);
			if (strcmp(var,"CORE_IP")==0) strcpy(CORE_IP,value);
			if (strcmp(var, "GPS_AHRS_IP") == 0) strcpy(GPS_AHRS_IP, value);
			if (strcmp(var, "ECHOLOGGER_IP") == 0) strcpy(ECHOLOGGER_IP, value);

			if (strcmp(var, "PORT_BOW_ACTUATOR_IP") == 0) strcpy(PORT_BOW_ACTUATOR_IP, value);
			if (strcmp(var, "PORT_STERN_ACTUATOR_IP") == 0) strcpy(PORT_STERN_ACTUATOR_IP, value);
			if (strcmp(var, "STBD_BOW_ACTUATOR_IP") == 0) strcpy(STBD_BOW_ACTUATOR_IP, value);
			if (strcmp(var, "STBD_STERN_ACTUATOR_IP") == 0) strcpy(STBD_STERN_ACTUATOR_IP, value);

			if (strcmp(var,"HEARTHBEAT_ROBOT_PORT_IN")==0) HEARTHBEAT_ROBOT_PORT_IN=(uint16)atoi(value);
			if (strcmp(var,"HEARTHBEAT_HMI_PORT_IN")==0) HEARTHBEAT_HMI_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"HEARTHBEAT_ROBOT_PORT_OUT")==0) HEARTHBEAT_ROBOT_PORT_OUT= (uint16)atoi(value);
			if (strcmp(var,"HEARTHBEAT_HMI_PORT_OUT")==0) HEARTHBEAT_HMI_PORT_OUT= (uint16)atoi(value);

			if (strcmp(var,"IO_CHANNELS_ROBOT_PORT_IN")==0) IO_CHANNELS_ROBOT_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"IO_CHANNELS_HMI_PORT_IN")==0) IO_CHANNELS_HMI_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"IO_CHANNELS_SIM_PORT_IN")==0) IO_CHANNELS_SIM_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"IO_CHANNELS_SIM_ROBOT_PORT_IN")==0) IO_CHANNELS_SIM_ROBOT_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"IO_CHANNELS_ROBOT_PORT_OUT")==0) IO_CHANNELS_ROBOT_PORT_OUT= (uint16)atoi(value);
			if (strcmp(var,"IO_CHANNELS_HMI_PORT_OUT")==0) IO_CHANNELS_HMI_PORT_OUT= (uint16)atoi(value);
			if (strcmp(var,"IO_CHANNELS_SIM_PORT_OUT")==0) IO_CHANNELS_SIM_PORT_OUT= (uint16)atoi(value);
			if (strcmp(var,"IO_CHANNELS_SIM_ROBOT_PORT_OUT")==0) IO_CHANNELS_SIM_ROBOT_PORT_OUT= (uint16)atoi(value);

			if (strcmp(var,"MOTORS_ROBOT_PORT_IN")==0) MOTORS_ROBOT_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"MOTORS_HMI_PORT_IN")==0) MOTORS_HMI_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"MOTORS_SIM_PORT_IN")==0) MOTORS_SIM_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"MOTORS_SIM_ROBOT_PORT_IN")==0) MOTORS_SIM_ROBOT_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"MOTORS_ROBOT_PORT_OUT")==0) MOTORS_ROBOT_PORT_OUT= (uint16)atoi(value);
			if (strcmp(var,"MOTORS_HMI_PORT_OUT")==0) MOTORS_HMI_PORT_OUT= (uint16)atoi(value);
			if (strcmp(var,"MOTORS_SIM_PORT_OUT")==0) MOTORS_SIM_PORT_OUT= (uint16)atoi(value);
			if (strcmp(var,"MOTORS_SIM_ROBOT_PORT_OUT")==0) MOTORS_SIM_ROBOT_PORT_OUT= (uint16)atoi(value);

			if (strcmp(var, "ACTUATOR_PORT_BOW_CMD_SIM_PORT_IN") == 0) ACTUATOR_PORT_BOW_CMD_SIM_PORT_IN = (uint16)atoi(value);
			if (strcmp(var, "ACTUATOR_PORT_STERN_CMD_SIM_PORT_IN") == 0) ACTUATOR_PORT_STERN_CMD_SIM_PORT_IN = (uint16)atoi(value);
			if (strcmp(var, "ACTUATOR_STBD_BOW_CMD_SIM_PORT_IN") == 0) ACTUATOR_STBD_BOW_CMD_SIM_PORT_IN = (uint16)atoi(value);
			if (strcmp(var, "ACTUATOR_STBD_STERN_CMD_SIM_PORT_IN") == 0) ACTUATOR_STBD_STERN_CMD_SIM_PORT_IN = (uint16)atoi(value);
			if (strcmp(var, "ACTUATOR_PORT_BOW_TLM_SIM_PORT_OUT") == 0) ACTUATOR_PORT_BOW_TLM_SIM_PORT_OUT = (uint16)atoi(value);
			if (strcmp(var, "ACTUATOR_PORT_STERN_TLM_SIM_PORT_OUT") == 0) ACTUATOR_PORT_STERN_TLM_SIM_PORT_OUT = (uint16)atoi(value);
			if (strcmp(var, "ACTUATOR_STBD_BOW_TLM_SIM_PORT_OUT") == 0) ACTUATOR_STBD_BOW_TLM_SIM_PORT_OUT = (uint16)atoi(value);
			if (strcmp(var, "ACTUATOR_STBD_STERN_TLM_SIM_PORT_OUT") == 0) ACTUATOR_STBD_STERN_TLM_SIM_PORT_OUT = (uint16)atoi(value);

			if (strcmp(var, "ACTUATOR_CMD_PORT_IN") == 0) ACTUATOR_CMD_PORT_IN = (uint16)atoi(value);
			if (strcmp(var, "ACTUATOR_TLM_PORT_OUT") == 0) ACTUATOR_TLM_PORT_OUT = (uint16)atoi(value);

			if (strcmp(var, "ACTUATOR_PB_CMD_ROBOT_PORT_OUT") == 0) ACTUATOR_PB_CMD_ROBOT_PORT_OUT = (uint16)atoi(value);
			if (strcmp(var, "ACTUATOR_PS_CMD_ROBOT_PORT_OUT") == 0) ACTUATOR_PS_CMD_ROBOT_PORT_OUT = (uint16)atoi(value);
			if (strcmp(var, "ACTUATOR_SB_CMD_ROBOT_PORT_OUT") == 0) ACTUATOR_SB_CMD_ROBOT_PORT_OUT = (uint16)atoi(value);
			if (strcmp(var, "ACTUATOR_SS_CMD_ROBOT_PORT_OUT") == 0) ACTUATOR_SS_CMD_ROBOT_PORT_OUT = (uint16)atoi(value);
			if (strcmp(var, "ACTUATOR_PB_TLM_ROBOT_PORT_IN") == 0) ACTUATOR_PB_TLM_ROBOT_PORT_IN = (uint16)atoi(value);
			if (strcmp(var, "ACTUATOR_PS_TLM_ROBOT_PORT_IN") == 0) ACTUATOR_PS_TLM_ROBOT_PORT_IN = (uint16)atoi(value);
			if (strcmp(var, "ACTUATOR_SB_TLM_ROBOT_PORT_IN") == 0) ACTUATOR_SB_TLM_ROBOT_PORT_IN = (uint16)atoi(value);
			if (strcmp(var, "ACTUATOR_SS_TLM_ROBOT_PORT_IN") == 0) ACTUATOR_SS_TLM_ROBOT_PORT_IN = (uint16)atoi(value);
			if (strcmp(var, "ACTUATOR_PB_TLM_ROBOT_PORT_OUT") == 0) ACTUATOR_PB_TLM_ROBOT_PORT_OUT = (uint16)atoi(value);
			if (strcmp(var, "ACTUATOR_PS_TLM_ROBOT_PORT_OUT") == 0) ACTUATOR_PS_TLM_ROBOT_PORT_OUT = (uint16)atoi(value);
			if (strcmp(var, "ACTUATOR_SB_TLM_ROBOT_PORT_OUT") == 0) ACTUATOR_SB_TLM_ROBOT_PORT_OUT = (uint16)atoi(value);
			if (strcmp(var, "ACTUATOR_SS_TLM_ROBOT_PORT_OUT") == 0) ACTUATOR_SS_TLM_ROBOT_PORT_OUT = (uint16)atoi(value);

			if (strcmp(var, "ACTUATOR_POW_EN_ROBOT_IN") == 0) ACTUATOR_POW_EN_ROBOT_IN = (uint16)atoi(value);
			if (strcmp(var, "ACTUATOR_POW_EN_HMI_OUT") == 0) ACTUATOR_POW_EN_HMI_OUT = (uint16)atoi(value);

			

			if (strcmp(var,"GPS_AHRS_ROBOT_PORT_IN")==0) GPS_AHRS_ROBOT_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"GPS_AHRS_HMI_PORT_IN")==0) GPS_AHRS_HMI_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"GPS_AHRS_SIM_PORT_IN")==0) GPS_AHRS_SIM_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"GPS_AHRS_SIM_ROBOT_PORT_IN")==0) GPS_AHRS_SIM_ROBOT_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"GPS_AHRS_ROBOT_PORT_OUT")==0) GPS_AHRS_ROBOT_PORT_OUT= (uint16)atoi(value);
			if (strcmp(var,"GPS_AHRS_HMI_PORT_OUT")==0) GPS_AHRS_HMI_PORT_OUT= (uint16)atoi(value);
			if (strcmp(var,"GPS_AHRS_SIM_PORT_OUT")==0) GPS_AHRS_SIM_PORT_OUT= (uint16)atoi(value);
			if (strcmp(var,"GPS_AHRS_SIM_ROBOT_PORT_OUT")==0) GPS_AHRS_SIM_ROBOT_PORT_OUT= (uint16)atoi(value);
			if (strcmp(var, "GPS_AHRS_PORT_IN") == 0) GPS_AHRS_PORT_IN = (uint16)atoi(value);
			if (strcmp(var, "GPS_AHRS_PORT_OUT") == 0) GPS_AHRS_PORT_OUT = (uint16)atoi(value);
			if (strcmp(var, "GPS_AHRS_ROBOT_HMI_PORT_OUT") == 0) GPS_AHRS_ROBOT_HMI_PORT_OUT = (uint16)atoi(value);

			if (strcmp(var,"FOG_ROBOT_PORT_IN")==0) FOG_ROBOT_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"FOG_HMI_PORT_IN")==0) FOG_HMI_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"FOG_SIM_PORT_IN")==0) FOG_SIM_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"FOG_SIM_ROBOT_PORT_IN")==0) FOG_SIM_ROBOT_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"FOG_ROBOT_PORT_OUT")==0) FOG_ROBOT_PORT_OUT= (uint16)atoi(value);
			if (strcmp(var,"FOG_HMI_PORT_OUT")==0) FOG_HMI_PORT_OUT= (uint16)atoi(value);
			if (strcmp(var,"FOG_SIM_PORT_OUT")==0) FOG_SIM_PORT_OUT= (uint16)atoi(value);
			if (strcmp(var,"FOG_SIM_ROBOT_PORT_OUT")==0) FOG_SIM_ROBOT_PORT_OUT= (uint16)atoi(value);

			if (strcmp(var,"DVL_ROBOT_PORT_IN")==0) DVL_ROBOT_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"DVL_HMI_PORT_IN")==0) DVL_HMI_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"DVL_SIM_PORT_IN")==0) DVL_SIM_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"DVL_SIM_ROBOT_PORT_IN")==0) DVL_SIM_ROBOT_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"DVL_ROBOT_PORT_OUT")==0) DVL_ROBOT_PORT_OUT= (uint16)atoi(value);
			if (strcmp(var,"DVL_HMI_PORT_OUT")==0) DVL_HMI_PORT_OUT= (uint16)atoi(value);
			if (strcmp(var,"DVL_SIM_PORT_OUT")==0) DVL_SIM_PORT_OUT= (uint16)atoi(value);
			if (strcmp(var,"DVL_SIM_ROBOT_PORT_OUT")==0) DVL_SIM_ROBOT_PORT_OUT= (uint16)atoi(value);

			if (strcmp(var,"PINGER_ROBOT_PORT_IN")==0) PINGER_ROBOT_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"PINGER_HMI_PORT_IN")==0) PINGER_HMI_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"PINGER_ROBOT_PORT_OUT")==0) PINGER_ROBOT_PORT_OUT= (uint16)atoi(value);
			if (strcmp(var,"PINGER_HMI_PORT_OUT")==0) PINGER_HMI_PORT_OUT= (uint16)atoi(value);

			if (strcmp(var,"CTD_ROBOT_PORT_IN")==0) CTD_ROBOT_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"CTD_HMI_PORT_IN")==0) CTD_HMI_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"CTD_SIM_PORT_IN")==0) CTD_SIM_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"CTD_SIM_ROBOT_PORT_IN")==0) CTD_SIM_ROBOT_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"CTD_ROBOT_PORT_OUT")==0) CTD_ROBOT_PORT_OUT= (uint16)atoi(value);
			if (strcmp(var,"CTD_HMI_PORT_OUT")==0) CTD_HMI_PORT_OUT= (uint16)atoi(value);
			if (strcmp(var,"CTD_SIM_PORT_OUT")==0) CTD_SIM_PORT_OUT= (uint16)atoi(value);
			if (strcmp(var,"CTD_SIM_ROBOT_PORT_OUT")==0) CTD_SIM_ROBOT_PORT_OUT= (uint16)atoi(value);

			if (strcmp(var,"PA500_ROBOT_PORT_IN")==0) PA500_ROBOT_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"PA500_HMI_PORT_IN")==0) PA500_HMI_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"PA500_SIM_PORT_IN")==0) PA500_SIM_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"PA500_SIM_ROBOT_PORT_IN")==0) PA500_SIM_ROBOT_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"PA500_ROBOT_PORT_OUT")==0) PA500_ROBOT_PORT_OUT= (uint16)atoi(value);
			if (strcmp(var,"PA500_HMI_PORT_OUT")==0) PA500_HMI_PORT_OUT= (uint16)atoi(value);
			if (strcmp(var,"PA500_SIM_PORT_OUT")==0) PA500_SIM_PORT_OUT= (uint16)atoi(value);
			if (strcmp(var,"PA500_SIM_ROBOT_PORT_OUT")==0) PA500_SIM_ROBOT_PORT_OUT= (uint16)atoi(value);

			if (strcmp(var,"ECHOLOGGER_ROBOT_PORT_IN")==0) ECHOLOGGER_ROBOT_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"ECHOLOGGER_PORT_OUT")==0) ECHOLOGGER_PORT_OUT = (uint16)atoi(value);

			if (strcmp(var,"NGC_ROBOT_PORT_IN")==0) NGC_ROBOT_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"NGC_HMI_PORT_IN")==0) NGC_HMI_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"NGC_TO_IO_PORT_OUT")==0) NGC_TO_IO_PORT_OUT= (uint16)atoi(value);
			if (strcmp(var,"NGC_ROBOT_PORT_OUT")==0) NGC_ROBOT_PORT_OUT= (uint16)atoi(value);
			if (strcmp(var,"NGC_HMI_PORT_OUT")==0) NGC_HMI_PORT_OUT= (uint16)atoi(value);

			if (strcmp(var,"TASKS_ROBOT_PORT_IN")==0) TASKS_ROBOT_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"TASKS_HMI_PORT_IN")==0) TASKS_HMI_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"TASKS_ROBOT_PORT_OUT")==0) TASKS_ROBOT_PORT_OUT= (uint16)atoi(value);
			if (strcmp(var,"TASKS_HMI_PORT_OUT")==0) TASKS_HMI_PORT_OUT= (uint16)atoi(value);

			if (strcmp(var,"PATHPLANNER_ROBOT_PORT_IN")==0) PATHPLANNER_ROBOT_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"PATHPLANNER_HMI_PORT_IN")==0) PATHPLANNER_HMI_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"PATHPLANNER_ROBOT_PORT_OUT")==0) PATHPLANNER_ROBOT_PORT_OUT= (uint16)atoi(value);
			if (strcmp(var,"PATHPLANNER_HMI_PORT_OUT")==0) PATHPLANNER_HMI_PORT_OUT= (uint16)atoi(value);

			if (strcmp(var,"CMD_ROBOT_PORT_IN")==0) CMD_ROBOT_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"CMD_ROBOT_PORT_OUT")==0) CMD_ROBOT_PORT_OUT= (uint16)atoi(value);
			if (strcmp(var,"CMD_SIM_PORT_IN")==0) CMD_SIM_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"CMD_SIM_PORT_OUT")==0) CMD_SIM_PORT_OUT= (uint16)atoi(value);
			if (strcmp(var,"TLM_ROBOT_PORT_IN")==0) TLM_ROBOT_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"TLM_ROBOT_PORT_OUT")==0) TLM_ROBOT_PORT_OUT= (uint16)atoi(value);
			if (strcmp(var,"TLM_SIM_PORT_IN")==0) TLM_SIM_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"TLM_SIM_PORT_OUT")==0) TLM_SIM_PORT_OUT= (uint16)atoi(value);

			if (strcmp(var,"PINGER_SIM_PORT_IN")==0) PINGER_SIM_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"PINGER_SIM_PORT_OUT")==0) PINGER_SIM_PORT_OUT= (uint16)atoi(value);
			if (strcmp(var,"USBL_SIM_PORT_IN")==0) USBL_SIM_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"USBL_SIM_PORT_OUT")==0) USBL_SIM_PORT_OUT= (uint16)atoi(value);
			if (strcmp(var,"USBL_POS_SIM_PORT_OUT")==0) USBL_POS_SIM_PORT_OUT= (uint16)atoi(value);
			if (strcmp(var,"USBL_POS_PORT_IN")==0) USBL_POS_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"USBL_POS_PORT_OUT")==0) USBL_POS_PORT_OUT= (uint16)atoi(value);
			if (strcmp(var,"USBL_TLM_PORT_OUT")==0) USBL_TLM_PORT_OUT= (uint16)atoi(value);
			if (strcmp(var,"USBL_HMI_TLM_PORT_IN")==0) USBL_HMI_TLM_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"USBL_CMD_PORT_IN")==0) USBL_CMD_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"USBL_HMI_CMD_PORT_OUT")==0) USBL_HMI_CMD_PORT_OUT= (uint16)atoi(value);

			if (strcmp(var,"CORE_ROBOT_PORT_IN")==0) CORE_ROBOT_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"CORE_ROBOT_PORT_OUT")==0) CORE_ROBOT_PORT_OUT= (uint16)atoi(value);
			if (strcmp(var,"CORE_DRIVER_PORT_IN")==0) CORE_DRIVER_PORT_IN= (uint16)atoi(value);
			if (strcmp(var,"CORE_DRIVER_PORT_OUT")==0) CORE_DRIVER_PORT_OUT= (uint16)atoi(value);

		}
		fclose(f);
		return 0;
	}
	else
	{
		printf("NetworkManager::init - File error: %s\n",name_dir_file);
		return -1;
	}
}


