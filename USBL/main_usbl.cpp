/*
 * main_usbl.cpp
 *
 *  Created on: May 9, 2018
 *      Author: mx
 */

#define __MAIN__

#include <stdio.h>
#include <stdlib.h>
#include <csignal>
#include <cstdlib>
#include <math.h>
#include <endian.h>
#include <iostream>
#include <iomanip>



#include "../common/generic/define.h"
#include "../common/network/CommLink.h"
#include "../common/network/NetworkManager.h"
#include "../common/data/Pinger_status.h"
#include "UsblLink.h"




#define TIMEOUT (3.0)   // [s]




bool loop_flag=true;

void ctrl_c_handler(int) { loop_flag=false; }



int main()
{
	char name_dir_file[256];
	sprintf(name_dir_file,"%s%s","../../../europe/config/",CONFIGURATION_NETWORK_FILE);
	strcpy(ROBOT_NAME,"USBL");

	NetworkManager networkManager;
	networkManager.init(name_dir_file);


	UsblLink *usbl=new UsblLink("USBL",networkManager);
	usbl->status(SEND);
	usbl->create();


	signal(SIGINT,ctrl_c_handler);

	timespec tSleep;
	tSleep.tv_sec= USBL_POS_SLEEP_SEC;
	tSleep.tv_nsec= USBL_POS_SLEEP_NSEC;


	char dato_str[100];

	double clock=0.0;


	CommLink* pos = new CommLink("USBL_pos", OVERRIDE);
	pos->open(networkManager.USBL_IP, networkManager.USBL_POS_PORT_OUT,
		networkManager.HMI_IP, networkManager.USBL_HMI_POS_PORT_IN);
	pos->create();


	CommLink *tlm=new CommLink( "USBL_tlm" , OVERRIDE);
	tlm->open( networkManager.USBL_IP , networkManager.USBL_TLM_PORT_OUT ,
			   networkManager.HMI_IP  , networkManager.USBL_HMI_TLM_PORT_IN );
	tlm->create();


	CommLink *cmd=new CommLink( "USBL_cmd" , OVERRIDE );
	cmd->open( networkManager.USBL_IP , networkManager.USBL_CMD_PORT_IN ,
			   networkManager.HMI_IP  , networkManager.USBL_HMI_CMD_PORT_OUT );
	cmd->create();



	CommLink* connCheck = new CommLink("connCheck", HARD_ACK);
	connCheck->open(networkManager.USBL_IP, networkManager.USBL_CHK_PORT_IN,
		            networkManager.HMI_IP, networkManager.USBL_HMI_CHK_PORT_OUT);
	connCheck->create();


	char cmd_to_send[BUF_SIZE];

	sprintf(cmd_to_send,"0");

	int brCheck,brCmd;
	char check_msg;


	while(loop_flag)
	{

		do {
			brCheck = connCheck->recv_message(&check_msg);
		} while (brCheck > 0);


		do {
			brCmd = cmd->recv_message(cmd_to_send);
			if (brCmd > 0) cmd_to_send[brCmd] = NULL;
		} while (brCmd > 0);


		//printf("Status: %d   %lf\n",usbl->status(),clock);

		if (usbl->status()==SEND)
		{
			//sprintf(dato_str,"%d",dato);
			//sprintf(dato_str,"abcdefghijklmnopqrstuvwxyz");
			usbl->sendUsblMessage(cmd_to_send);
			//printf("Dato trasmesso: %s    %d\n", cmd_to_send,strlen(cmd_to_send));
		}

		if (usbl->status()==SENT)
		{
			usbl->status(RECEIVE);
			clock=0.0;
			sprintf(cmd_to_send, "0");
		}


		if((usbl->status()==RECEIVE || usbl->status()==RECEIVING) && clock>=TIMEOUT)
		{
			//printf("**************** status: %d    clock: %lf\n",usbl->status(),clock);
			printf("*** NOT PINGING! ***\n");
			usbl->status(SEND);
		}


		if (usbl->status()==RECEIVED)
		{
			//dato=strtoul(usbl->receiveUsblMessage(),NULL,10);
			char *s=usbl->receiveUsblMessage();

			//printf("Dato ricevuto: %d     x: %lf   y: %lf   z: %lf\n",dato,usbl->getPosX(),usbl->getPosY(),usbl->getPosZ());
			//printf("Dato ricevuto: %s     x: %lf   y: %lf   z: %lf\n",s,usbl->getPosX(),usbl->getPosY(),usbl->getPosZ());
			//dato++;
			usbl->status(SEND);
		
			USBLpos_packet posPacket;
			posPacket.x=(int64)(usbl->getPosX() * USBLpos_factor);
			posPacket.y=(int64)(usbl->getPosY() * USBLpos_factor);
			posPacket.z=(int64)(usbl->getPosZ() * USBLpos_factor);

			pos->send_message((char*)&posPacket,sizeof(posPacket));



			if (strlen(s) == 1 && s[0] == '0')
			{
				//printf("Arrivato %s\n", s);
			}
			else
			{
				//printf("Sending tlm\n");
				tlm->send_message(s, strlen(s));
			}

			
		}
		nanosleep(&tSleep,NULL);

		clock+=(double)tSleep.tv_sec + ((double)tSleep.tv_nsec)/1000000000.0;

	}

	cmd->terminate();
	delete cmd;

	tlm->terminate();
	delete tlm;

	connCheck->terminate();
	delete connCheck;

	usbl->terminate();
	delete usbl;

}
