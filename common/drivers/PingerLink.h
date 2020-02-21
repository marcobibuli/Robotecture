/*
 * PingerLink.h
 *
 *  Created on: May 9, 2018
 *      Author: mx
 */

#ifndef PINGERLINK_H_
#define PINGERLINK_H_

#include "../generic/define.h"
#include "../generic/custom_types.h"
#include "../drivers/Device.h"
#include "../network/CommLink.h"
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <vector>
#include <semaphore.h>
//#include <boost/asio.hpp>

//using namespace boost;


#define BUF_SIZE 2048
#define DEFAULT_INTERVAL 200000
#define USBL_POWER_LEVEL 3
#define USBL_WAVE_FORM 0
#define USBL_ADDR 1
#define USBL_EXT_NOTIFICATION 1



enum PingerLinkProtocol
{
	SEND=0,
	SENDING,
	SENT,
	SEND_ERROR,
	RECEIVE,
	RECEIVING,
	RECEIVED
};


struct PingerMessage
{
	unsigned int timeStamp;
	char data[BUF_SIZE];
};

struct Pinger_sim_packet
{
	char data[BUF_SIZE];
};




class PingerLink:public Device
{
	private:
		int serialPort;
		PingerMessage msgIn;
		PingerMessage msgOut;
		PingerLinkProtocol pinger_status;

		CommLink *inLink,*outLink;

		sem_t statusMutex;

		double range;
		char sequence;
		double pos_x,pos_y,pos_z;

		char* extractPosition(char *dataBuf);


	public:
		PingerLink(const char *name,NetworkManager &nm);
		~PingerLink();

		//void execute();

		char* receivePingerMessage();
		void sendPingerMessage(char* m);
		void sendPingerMessage_sim(char* buf,int bufLength);
		void sendPingerMessage_act(char* buf);

		void pingerStatus(PingerLinkProtocol s){sem_wait(&statusMutex); pinger_status=s; sem_post(&statusMutex);}
		PingerLinkProtocol pingerStatus(){sem_wait(&statusMutex); PingerLinkProtocol s=pinger_status; sem_post(&statusMutex); return s;}

		virtual int init_sim();
		virtual int init_act();
		int init_pinger();

		virtual void execute_sim();
		virtual void execute_act();

		void getPosition(double &x,double &y,double &z);
};

void* start_PingerLink(void *arg);


#endif /* PINGERLINK_H_ */
