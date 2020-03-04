/*
 * UsblLink.h
 *
 *  Created on: May 9, 2018
 *      Author: mx
 */

#ifndef USBLLINK_H_
#define USBLLINK_H_

#include "../common/data/Pinger_status.h"
#include "../common/generic/RobotThread.h"
#include "../common/generic/RobotMath.h"
#include "../common/network/CommLink.h"
#include "../common/network/NetworkManager.h"
#include "../common/drivers/Device.h"
#include "client.h"
#include <vector>
#include <string.h>
#include <semaphore.h>
#include <stdlib.h>


#define configDir "../../config/USBL/"
#define configFileName "USBL.cfg"



#define USBL_IP_ADDR "10.17.20.133"
#define USBL_PORT 9200
#define DEFAULT_INTERVAL 200000
#define USBL_POWER_LEVEL 3
#define USBL_WAVE_FORM 0
#define PINGER_ADDR 2
#define USBL_EXT_NOTIFICATION 1



#define getExtNotification	"AT?ZX"
#define setExtNotification	"AT@ZX"
#define getPowerLevel		"AT?L"
#define setPowerLevel		"AT!L"
#define getWaveForm			"AT?C"
#define setWaveForm   	 	"AT!C"
#define getFWVersion     	"ATI0"
#define getFWVersionMinor	"ATI1"
#define getFWVersionBrand 	"ATI7"
#define sendMessage  		"AT*SENDIM"
#define getUSBLPosition   	"AT?UP"
#define getUSBLRetryIM   	"AT?RI"
#define setUSBLRetryIM   	"AT!RI1"



enum USBLLinkProtocol
{
	SEND=0,
	SENDING,
	SENT,
	SEND_ERROR,
	RECEIVE,
	RECEIVING,
	RECEIVED
};



struct UsblMessage
{
	unsigned int timeStamp;
	char data[BUF_SIZE];
};


struct USBL_sim_packet
{
	char data[BUF_SIZE];
};




class UsblLink:public RobotThread
{
	private:
		Client *cl=NULL;
		UsblMessage msgIn;
		UsblMessage msgOut;
		unsigned int timeStampSend;
		USBLLinkProtocol usblStatus;

		CommLink *inLink,*outLink,*posLink;

		sem_t statusMutex;

		double range;
		char sequence;

		double pos_x,pos_y,pos_z;

		DeviceMode device_mode;

		NetworkManager *networkManager;


		int read_from_usbl(char const *cmd,char *out);
		int write_to_usbl(char const *cmd,int value,char *out);

	public:
		UsblLink(const char *name,NetworkManager &nm);
		~UsblLink();

		int init_sim();
		int init_act();

		void execute();
		void execute_sim();
		void execute_act();

		char* receiveUsblMessage();
		void sendUsblMessage(char* m);
		void sendUsblMessage_act(char* buf);
		void sendUsblMessage_sim(char* buf,int bufLength);

		void status(USBLLinkProtocol s){sem_wait(&statusMutex); usblStatus=s; sem_post(&statusMutex);}
		USBLLinkProtocol status(){sem_wait(&statusMutex); USBLLinkProtocol s=usblStatus; sem_post(&statusMutex); return s;}

		double getRange()const{return range;}
		double getPosX()const{return pos_x;}
		double getPosY()const{return pos_y;}
		double getPosZ()const{return pos_z;}

};

void* start_UsblLink(void *arg);


#endif /* USBLLINK_H_ */
