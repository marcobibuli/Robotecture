/*
 * CTD.cpp
 *
 *  Created on: Jan 28, 2019
 *      Author: mx
 */


#include "CTD.h"

CTD::CTD(const char *name,NetworkManager &nm, DataAccess<CTD_status>& CTD_access, DataAccess<Time_status>& Time_access):Device(name,SCHED_FIFO,CTD_THREAD_PRIORITY,start_ctd,nm)
{
	networkManager=&nm;

	time_access = &Time_access;
	ctd_access = &CTD_access;


	strcpy(configFileName,CONFIGURATION_CTD_FILE);


/*
	tlm=new CommLink( "FOG_tlm" , OVERRIDE );
	tlm->open( networkManager->ROBOT_IP , networkManager->FOG_ROBOT_PORT_OUT ,
			   networkManager->HMI_IP   , networkManager->FOG_HMI_PORT_IN );
	tlm->create();
	*/

	

	conductivity=0.0;
	temperature=0.0;
	depth=0.0;
}

CTD::~CTD()
{

}


int CTD::init_sim()
{
	printf("CTD::init_sim()\n");
	running_sim=true;
	device_status=DEVICE_RUNNING;

	tlmSim = new CommLink("FOG_tlmSim", UDP_PURE);
	tlmSim->open(networkManager->ROBOT_IP, networkManager->CTD_ROBOT_SIM_PORT_IN,
		         networkManager->SIM_IP, networkManager->CTD_SIM_PORT_OUT);
	tlmSim->create();

	return 0;
}


int CTD::init_act()
{
	printf("CTD::init_act()\n");
	running_act=true;

	strcpy(ctd304PlusSerialDeviceName,ctd_serial_name);

	if(ctd_304_plus_init(&ctd304Plus,ctd304PlusSerialDeviceName)<0)
	{
		printf("CTD init error.\n");
		return(-1);
	}

	device_status=DEVICE_RUNNING;

	return 0;
}


void CTD::execute_sim()
{
	timespec tSleep;
	tSleep.tv_sec=CTD_SLEEP_SEC;
	tSleep.tv_nsec=CTD_SLEEP_NSEC;

	while(running_sim)
	{
		read_sim_tlm();

		updateStatus();

		//send_telemetry();

		nanosleep(&tSleep,NULL);
	}
}


void CTD::read_sim_tlm()
{
	CTD_tlm_packet msg;
	int ret;
	do{
		ret=tlmSim->recv_message((char*)&msg);
		if (ret>0)
		{
			conductivity=(((double)(msg.conductivity))/CTD_factor);
			temperature=(((double)(msg.temperature))/CTD_factor);
			depth=(((double)(msg.depth))/CTD_factor);
		}

		update_device_status(ret);

	}while(ret>0);

}


void CTD::execute_act()
{
	timespec tSleep;
	tSleep.tv_sec=CTD_SLEEP_SEC;
	tSleep.tv_nsec=CTD_SLEEP_NSEC;

	while(running_act)
	{
		int ret=ctd_304_plus_read(&ctd304Plus,&conductivity,&temperature,&depth);
		if (ret == 0) ret = 1;

		update_device_status(ret);

		updateStatus();

		//send_telemetry();

		nanosleep(&tSleep,NULL);
	}
}


void CTD::update_device_status(int r)
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
		if (missed_receive_count>CTD_RECEIVE_WAIT_LOOPS_FOR_WARNING && missed_receive_count<=CTD_RECEIVE_WAIT_LOOPS_FOR_FAULT)
			device_status=DEVICE_WARNING;

		else if (missed_receive_count>CTD_RECEIVE_WAIT_LOOPS_FOR_FAULT)
		{
			device_status=DEVICE_FAULT;
			missed_receive_count=CTD_RECEIVE_WAIT_LOOPS_FOR_FAULT;
		}
	}
}


void CTD::updateStatus()
{
	Time_status ts;
	ts=time_access->get();

	if (missed_receive_count==0) lastValidTimeStamp=ts.timeStamp;

	CTD_status ctd_status;
	ctd_status=ctd_access->get();

	ctd_status.conductivity.value = conductivity;		ctd_status.conductivity.timeStamp = lastValidTimeStamp;      ctd_status.conductivity.valid = true;
	ctd_status.temperature.value=temperature;			ctd_status.temperature.timeStamp = lastValidTimeStamp;		 ctd_status.temperature.valid = true;
	ctd_status.depth.value=depth;						ctd_status.depth.timeStamp = lastValidTimeStamp;			 ctd_status.depth.valid = true;
	ctd_status.device_status=device_status;
	ctd_status.timeStamp=lastValidTimeStamp;

	ctd_access->set(ctd_status);
}


void CTD::send_telemetry()
{
	CTD_tlm_packet msg;

	CTD_status ctd_status;
	ctd_status = ctd_access->get();

	ctd_status.compose_tlm_packet(msg);

/*
	msg.roll=(int64)((RobotNavData->roll * M_PI/180.0 )*FOG_factor);
	msg.pitch=(int64)((RobotNavData->pitch * M_PI/180.0 )*FOG_factor);
	msg.heading=(int64)((mod180(RobotNavData->heading * M_PI/180.0 ))*FOG_factor);
	msg.rollRate=(int64)((RobotNavData->rollRate * M_PI/180.0 )*FOG_factor);
	msg.pitchRate=(int64)((RobotNavData->pitchRate * M_PI/180.0 )*FOG_factor);
	msg.headingRate=(int64)((mod180(RobotNavData->headingRate * M_PI/180.0 ))*FOG_factor);
	msg.device_status=device_status;
*/
	tlm->send_message((char*)&msg,sizeof(msg));
}


void* start_ctd(void *arg)
{
	CTD* pThread=(CTD*)((ThreadInfo*)arg)->pThread;
	pThread->execute();
	return NULL;
}
