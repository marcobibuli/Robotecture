/*
 * NetworkManager.h
 *
 *  Created on: Nov 9, 2016
 *      Author: mx
 */

#ifndef NETWORK_NETWORKMANAGER_H_
#define NETWORK_NETWORKMANAGER_H_

#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include "../generic/custom_types.h"
#include "../generic/define.h"


class NetworkManager
{
	public:
		uint16 HEARTHBEAT_ROBOT_PORT_IN;
		uint16 HEARTHBEAT_HMI_PORT_IN;
		uint16 HEARTHBEAT_ROBOT_PORT_OUT;
		uint16 HEARTHBEAT_HMI_PORT_OUT;

		uint16 IO_ROBOT_SIM_PORT_IN;
		uint16 IO_ROBOT_SIM_PORT_OUT;
		uint16 IO_ROBOT_HMI_PORT_IN;
		uint16 IO_ROBOT_HMI_PORT_OUT;
		uint16 IO_SIM_PORT_IN;
		uint16 IO_SIM_PORT_OUT;
		uint16 IO_HMI_PORT_IN;
		uint16 IO_HMI_PORT_OUT;

	


		uint16 GPS_AHRS_ROBOT_SIM_PORT_IN;
		uint16 GPS_AHRS_ROBOT_SIM_PORT_OUT;
		uint16 GPS_AHRS_ROBOT_HMI_PORT_IN;
		uint16 GPS_AHRS_ROBOT_HMI_PORT_OUT;
		uint16 GPS_AHRS_SIM_PORT_IN;
		uint16 GPS_AHRS_SIM_PORT_OUT;	
		uint16 GPS_AHRS_HMI_PORT_IN;
		uint16 GPS_AHRS_HMI_PORT_OUT;

		uint16 FOG_ROBOT_SIM_PORT_IN;
		uint16 FOG_ROBOT_SIM_PORT_OUT;
		uint16 FOG_ROBOT_HMI_PORT_IN;
		uint16 FOG_ROBOT_HMI_PORT_OUT;
		uint16 FOG_SIM_PORT_IN;
		uint16 FOG_SIM_PORT_OUT;
		uint16 FOG_HMI_PORT_IN;
		uint16 FOG_HMI_PORT_OUT;

		uint16 DVL_ROBOT_SIM_PORT_IN;
		uint16 DVL_ROBOT_SIM_PORT_OUT;
		uint16 DVL_ROBOT_HMI_PORT_IN;
		uint16 DVL_ROBOT_HMI_PORT_OUT;
		uint16 DVL_SIM_PORT_IN;
		uint16 DVL_SIM_PORT_OUT;
		uint16 DVL_HMI_PORT_IN;
		uint16 DVL_HMI_PORT_OUT;

		uint16 CTD_ROBOT_SIM_PORT_IN;
		uint16 CTD_ROBOT_SIM_PORT_OUT;
		uint16 CTD_ROBOT_HMI_PORT_IN;
		uint16 CTD_ROBOT_HMI_PORT_OUT;
		uint16 CTD_SIM_PORT_IN;
		uint16 CTD_SIM_PORT_OUT;
		uint16 CTD_HMI_PORT_IN;
		uint16 CTD_HMI_PORT_OUT;

		uint16 PA500_ROBOT_SIM_PORT_IN;
		uint16 PA500_ROBOT_SIM_PORT_OUT;
		uint16 PA500_ROBOT_HMI_PORT_IN;
		uint16 PA500_ROBOT_HMI_PORT_OUT;
		uint16 PA500_SIM_PORT_IN;
		uint16 PA500_SIM_PORT_OUT;
		uint16 PA500_HMI_PORT_IN;
		uint16 PA500_HMI_PORT_OUT;

		uint16 ECHOLOGGER_ROBOT_SIM_PORT_IN;
		uint16 ECHOLOGGER_ROBOT_SIM_PORT_OUT;
		uint16 ECHOLOGGER_ROBOT_HMI_PORT_IN;
		uint16 ECHOLOGGER_ROBOT_HMI_PORT_OUT;
		uint16 ECHOLOGGER_SIM_PORT_IN;
		uint16 ECHOLOGGER_SIM_PORT_OUT;
		uint16 ECHOLOGGER_HMI_PORT_IN;
		uint16 ECHOLOGGER_HMI_PORT_OUT;

		uint16 PINGER_ROBOT_SIM_PORT_IN;
		uint16 PINGER_ROBOT_SIM_PORT_OUT;
		uint16 PINGER_ROBOT_HMI_PORT_IN;
		uint16 PINGER_ROBOT_HMI_PORT_OUT;
		uint16 PINGER_SIM_PORT_IN;
		uint16 PINGER_SIM_PORT_OUT;
		uint16 PINGER_HMI_PORT_IN;
		uint16 PINGER_HMI_PORT_OUT;

		
		uint16 USBL_HMI_TLM_PORT_IN;
		uint16 USBL_HMI_TLM_PORT_OUT;
		uint16 USBL_HMI_CMD_PORT_IN;
		uint16 USBL_HMI_CMD_PORT_OUT;
		uint16 USBL_TLM_PORT_IN;
		uint16 USBL_TLM_PORT_OUT;
		uint16 USBL_CMD_PORT_IN;
		uint16 USBL_CMD_PORT_OUT;
		uint16 USBL_POS_SIM_PORT_OUT;
		uint16 USBL_POS_PORT_IN;
		uint16 USBL_SIM_PORT_IN;
		uint16 USBL_SIM_PORT_OUT;



		uint16 NGC_ROBOT_HMI_PORT_IN;
		uint16 NGC_ROBOT_HMI_PORT_OUT;
		uint16 NGC_HMI_PORT_IN;
		uint16 NGC_HMI_PORT_OUT;

		uint16 TASKS_ROBOT_HMI_PORT_IN;
		uint16 TASKS_ROBOT_HMI_PORT_OUT;
		uint16 TASKS_HMI_PORT_IN;
		uint16 TASKS_HMI_PORT_OUT;
		







		uint16 MOTORS_ROBOT_PORT_IN;
		uint16 MOTORS_HMI_PORT_IN;
		uint16 MOTORS_ROBOT_PORT_OUT;
		uint16 MOTORS_HMI_PORT_OUT;
		uint16 MOTORS_SIM_PORT_IN;
		uint16 MOTORS_SIM_PORT_OUT;
		uint16 MOTORS_SIM_ROBOT_PORT_IN;
		uint16 MOTORS_SIM_ROBOT_PORT_OUT;


		uint16 ACTUATOR_PORT_BOW_CMD_SIM_PORT_IN;
		uint16 ACTUATOR_PORT_STERN_CMD_SIM_PORT_IN;
		uint16 ACTUATOR_STBD_BOW_CMD_SIM_PORT_IN;
		uint16 ACTUATOR_STBD_STERN_CMD_SIM_PORT_IN;
		uint16 ACTUATOR_PORT_BOW_TLM_SIM_PORT_OUT;
		uint16 ACTUATOR_PORT_STERN_TLM_SIM_PORT_OUT;
		uint16 ACTUATOR_STBD_BOW_TLM_SIM_PORT_OUT;
		uint16 ACTUATOR_STBD_STERN_TLM_SIM_PORT_OUT;

		uint16 ACTUATOR_CMD_PORT_IN;
		uint16 ACTUATOR_TLM_PORT_OUT;
		uint16 ACTUATOR_PB_CMD_ROBOT_PORT_OUT;
		uint16 ACTUATOR_PS_CMD_ROBOT_PORT_OUT;
		uint16 ACTUATOR_SB_CMD_ROBOT_PORT_OUT;
		uint16 ACTUATOR_SS_CMD_ROBOT_PORT_OUT;
		uint16 ACTUATOR_PB_TLM_ROBOT_PORT_IN;
		uint16 ACTUATOR_PS_TLM_ROBOT_PORT_IN;
		uint16 ACTUATOR_SB_TLM_ROBOT_PORT_IN;
		uint16 ACTUATOR_SS_TLM_ROBOT_PORT_IN;
		uint16 ACTUATOR_PB_TLM_ROBOT_PORT_OUT;
		uint16 ACTUATOR_PS_TLM_ROBOT_PORT_OUT;
		uint16 ACTUATOR_SB_TLM_ROBOT_PORT_OUT;
		uint16 ACTUATOR_SS_TLM_ROBOT_PORT_OUT;

		uint16 ACTUATOR_POW_EN_ROBOT_IN;
		uint16 ACTUATOR_POW_EN_HMI_OUT;


		

	

		uint16 PATHPLANNER_ROBOT_PORT_IN;
		uint16 PATHPLANNER_HMI_PORT_IN;
		uint16 PATHPLANNER_ROBOT_PORT_OUT;
		uint16 PATHPLANNER_HMI_PORT_OUT;

		uint16 CMD_ROBOT_PORT_IN;
		uint16 CMD_ROBOT_PORT_OUT;
		uint16 CMD_SIM_PORT_IN;
		uint16 CMD_SIM_PORT_OUT;
		uint16 TLM_ROBOT_PORT_IN;
		uint16 TLM_ROBOT_PORT_OUT;
		uint16 TLM_HMI_PORT_IN;
		uint16 TLM_HMI_PORT_OUT;

		
		

		uint16 CORE_ROBOT_PORT_IN;
		uint16 CORE_ROBOT_PORT_OUT;
		uint16 CORE_DRIVER_PORT_IN;
		uint16 CORE_DRIVER_PORT_OUT;

		char ROBOT_IP[20];
		char HMI_IP[20];
		char SIM_IP[20];
		char USBL_IP[20];
		char CORE_IP[20];
		char PORT_BOW_ACTUATOR_IP[20];
		char PORT_STERN_ACTUATOR_IP[20];
		char STBD_BOW_ACTUATOR_IP[20];
		char STBD_STERN_ACTUATOR_IP[20];
		char GPS_AHRS_IP[20];
		char ECHOLOGGER_IP[20];


		NetworkManager();
		~NetworkManager();

		int init();
		int init(const char *name_dir_file);
};


#endif /* NETWORK_NETWORKMANAGER_H_ */
