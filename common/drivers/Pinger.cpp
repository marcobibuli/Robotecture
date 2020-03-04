/*
 * Pinger.cpp
 *
 *  Created on: May 25, 2018
 *      Author: mx
 */

#include "Pinger.h"

Pinger::Pinger(const char *name,NetworkManager &nm, DataAccess<Pinger_status>& Pinger_access, DataAccess<Time_status>& Time_access):Device(name,SCHED_FIFO,PINGER_THREAD_PRIORITY,start_pinger,nm)
{
	strcpy(configFileName,CONFIGURATION_PINGER_FILE);
	pinger=NULL;
	m=NULL;

	time_access = &Time_access;
	pinger_access = &Pinger_access;

	Pinger_status pinger_status;
	pinger_status = pinger_access->get();
	

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
	pinger=new PingerLink("PingerLink",*networkManager);
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

	Pinger_status pinger_status;
	pinger_status = pinger_access->get();
	

	device_status=DEVICE_OFF;
	return 0;
}


void Pinger::execute_pinger()
{
	timespec tSleep;
	tSleep.tv_sec=PINGER_SLEEP_SEC;
	tSleep.tv_nsec=PINGER_SLEEP_NSEC;

	char dato_str[100];

	Pinger_status pinger_status;

	int st;

	

	while(running)
	{
		st=0;
		
		pinger_status = pinger_access->get();
		
		if (pinger_status.powered==0)
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

		if (pinger_status.powered ==1 && device_status==DEVICE_OFF)
		{
			init_pinger();
			if (pinger->init()==0) pinger->create();

			device_status=DEVICE_INIT;
		}

		if (pinger_status.powered ==1 && device_status!=DEVICE_OFF)
		{
			//device_status=DEVICE_RUNNING;

			if (pinger->pingerStatus()==SEND)
			{
				//sprintf(dato_str,"qazwscedcrfvtgbyhnujmikolp");
				strcpy(dato_str, pinger_status.acoustic_tlm);
				pinger->sendPingerMessage(dato_str);
				//printf("sending: %s\n", dato_str);
				st=1;
			}

			if (pinger->pingerStatus()==SENT)
			{
				pinger->pingerStatus(RECEIVE);
				st=0;
			}

			if (pinger->pingerStatus()==RECEIVED)
			{
				//strcpy(pinger_status.acoustic_cmd, pinger->receivePingerMessage());
				if ((pinger->receivePingerMessage())[0]!='0') pinger_status.add_cmd(pinger->receivePingerMessage());
				//char *s=pinger->receivePingerMessage();
				//printf("Cmd ricevuto: %s\n", pinger_status.acoustic_cmd[pinger_status.acoustic_cmd.size()-1]);
				//printf("Cmd ricevuto: %s\n", pinger->receivePingerMessage());
				pinger->pingerStatus(SEND);
				st=1;
			}

			update_device_status(st);

		}

		pinger_access->set(pinger_status);

		updateStatus();

		//send_telemetry();
		
		nanosleep(&tSleep,NULL);
	}
}


void Pinger::update_device_status(int r)
{
	if (r>0 && device_status != DEVICE_OFF)
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
	ts=time_access->get();

	if (missed_receive_count==0) lastValidTimeStamp=ts.timeStamp;

	Pinger_status pinger_status;
	pinger_status=pinger_access->get();

	if (device_status!=DEVICE_OFF)
	{

		double x=0.0,y=0.0,z=0.0;
		if (pinger!=NULL) pinger->getPosition(x,y,z);
		pinger_status.x.value=x;	pinger_status.x.valid = true;	pinger_status.x.timeStamp = lastValidTimeStamp;
		pinger_status.y.value=y;	pinger_status.y.valid = true;	pinger_status.y.timeStamp = lastValidTimeStamp;
		pinger_status.z.value=z;	pinger_status.z.valid = true;	pinger_status.z.timeStamp = lastValidTimeStamp;

		pinger_status.timeStamp=lastValidTimeStamp;
	}

	pinger_status.device_status=device_status;
	pinger_access->set(pinger_status);

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
	/*
	Pinger_tlm_packet msg;

	msg.device_status=device_status;

	tlm->send_message((char*)&msg,sizeof(msg));
	*/
}



void* start_pinger(void *arg)
{
	Pinger* pThread=(Pinger*)((ThreadInfo*)arg)->pThread;
	pThread->execute();
	return NULL;
}
