/*
 * Pinger.cpp
 *
 *  Created on: May 25, 2018
 *      Author: mx
 */

#include "Pinger.h"

Pinger::Pinger(const char *name,NetworkManager &nm,Status *s):Device(name,SCHED_FIFO,PINGER_THREAD_PRIORITY,start_pinger,nm,s)
{
	strcpy(configFileName,CONFIGURATION_PINGER_FILE);
	pinger=NULL;
	m=NULL;

	/*
	tlm=new CommLink( "Pinger_tlm" , OVERRIDE );
	tlm->open( networkManager->ROBOT_IP , networkManager->PINGER_ROBOT_PORT_OUT ,
			networkManager->HMI_IP   , networkManager->PINGER_HMI_PORT_IN );
	tlm->create();
	*/
}

Pinger::~Pinger()
{
	if (pinger!=NULL) pinger->terminate();
}


int Pinger::init_pinger()
{
	printf("Pinger::init_pinger()\n");
	pinger=new PingerLink("PingerLink",*networkManager,status);
	pinger->pingerStatus(RECEIVE);

	//pinger->init();

	//if (pinger->init()==0) pinger->create();

	return 0;
}

int Pinger::init_act()
{
	printf("Pinger::init_act()\n");
	//return init_pinger();

	device_status=DEVICE_OFF;
	return 0;
}

int Pinger::init_sim()
{
	printf("Pinger::init_sim()\n");
	//return init_pinger();

	device_status=DEVICE_OFF;
	return 0;
}


void Pinger::execute_pinger()
{
	timespec tSleep;
	tSleep.tv_sec=PINGER_SLEEP_SEC;
	tSleep.tv_nsec=PINGER_SLEEP_NSEC;

	char dato_str[100];

	IO_status io_status;

	int st;

	while(running)
	{
		st=1;

		io_status=status->io_status.get();

		if (io_status.tlm.digital[EUROPE_DIO_EVOLOGICS_MODEM_POWER]==0)
		{
			device_status=DEVICE_OFF;
			if (pinger!=NULL)
			{
				pinger->terminate();
				while(!(pinger->isTerminated())) {nanosleep(&tSleep,NULL); }
				delete pinger;
				pinger=NULL;
			}
		}

		if ((io_status.tlm.digital)[EUROPE_DIO_EVOLOGICS_MODEM_POWER]==1 && device_status==DEVICE_OFF)
		{
			init_pinger();
			if (pinger->init()==0) pinger->create();

			device_status=DEVICE_INIT;
		}

		if (io_status.tlm.digital[EUROPE_DIO_EVOLOGICS_MODEM_POWER]==1 && device_status!=DEVICE_OFF)
		{
			device_status=DEVICE_RUNNING;

			if (pinger->pingerStatus()==SEND)
			{
				sprintf(dato_str,"qazwscedcrfvtgbyhnujmikolp");
				pinger->sendPingerMessage(dato_str);
				st=1;
			}

			if (pinger->pingerStatus()==SENT)
			{
				pinger->pingerStatus(RECEIVE);
				st=0;
			}

			if (pinger->pingerStatus()==RECEIVED)
			{
				char *s=pinger->receivePingerMessage();
				//printf("Dato ricevuto: %s\n",s);
				pinger->pingerStatus(SEND);
				st=1;
			}

			update_device_status(st);

		}

		updateStatus();

		//send_telemetry();

		nanosleep(&tSleep,NULL);
	}
}


void Pinger::update_device_status(int r)
{
	if (r>0)
	{
		missed_receive_count=0;
		device_status=DEVICE_RUNNING;
	}

	else if (r <= 0)
	{
		//printf("FOG - ERROR reading message.\n");
		missed_receive_count++;
		if (missed_receive_count>PINGER_RECEIVE_WAIT_LOOPS_FOR_WARNING && missed_receive_count<=PINGER_RECEIVE_WAIT_LOOPS_FOR_FAULT)
			device_status=DEVICE_WARNING;

		else if (missed_receive_count>PINGER_RECEIVE_WAIT_LOOPS_FOR_FAULT)
		{
			device_status=DEVICE_FAULT;
			missed_receive_count=PINGER_RECEIVE_WAIT_LOOPS_FOR_FAULT;
		}
	}
}


void Pinger::updateStatus()
{
	Time_status ts;
	ts=status->time_status.get();

	if (missed_receive_count==0) lastValidTimeStamp=ts.timeStamp;

	USBL_status pinger_status;
	pinger_status=status->pinger_status.get();

	if (device_status!=DEVICE_OFF)
	{

		double x=0.0,y=0.0,z=0.0;
		if (pinger!=NULL) pinger->getPosition(x,y,z);
		pinger_status.x.value=x;
		pinger_status.y.value=y;
		pinger_status.z.value=z;

		pinger_status.timeStamp=lastValidTimeStamp;
	}

	pinger_status.device_status=device_status;
	status->pinger_status.set(pinger_status);
}



void Pinger::execute_sim()
{
	execute_pinger();
}

void Pinger::execute_act()
{
	execute_pinger();
}


void Pinger::send_telemetry()
{
	Pinger_tlm_packet msg;

	msg.device_status=device_status;

	tlm->send_message((char*)&msg,sizeof(msg));
}



void* start_pinger(void *arg)
{
	Pinger* pThread=(Pinger*)((ThreadInfo*)arg)->pThread;
	pThread->execute();
	return NULL;
}
