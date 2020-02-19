/*
 * PA500.cpp
 *
 *  Created on: Jul 24, 2018
 *      Author: mx
 */



#include "PA500.h"

PA500alt::PA500alt(const char *name,NetworkManager &nm,Status *s):Device(name,SCHED_FIFO,PA500_THREAD_PRIORITY,start_pa500alt,nm,s)
{
	strcpy(configFileName,CONFIGURATION_PA500_FILE);

	strcpy(pa500SerialDeviceName,pa500_serial_name);

	range=-1.0;
/*
	tlm=new CommLink( "PA500_tlm" , OVERRIDE , sizeof(PA500_tlm_packet) );
	tlm->open( networkManager->ROBOT_IP , networkManager->PA500_ROBOT_PORT_OUT ,
			   networkManager->HMI_IP   , networkManager->PA500_HMI_PORT_IN );
	tlm->create();
*/
	tlmSim=new CommLink( "PA500_tlmSim" , OVERRIDE );
	tlmSim->open( networkManager->ROBOT_IP , networkManager->PA500_SIM_ROBOT_PORT_IN ,
			      networkManager->SIM_IP   , networkManager->PA500_SIM_PORT_OUT );
	tlmSim->create();

	pa500=NULL;

	measure_flag=0;
}

PA500alt::~PA500alt()
{

}


int PA500alt::init_sim()
{
	printf("PA500::init_sim()\n");
	running_sim=true;

	device_status=DEVICE_OFF;

	return 0;
}


int PA500alt::init_act()
{
	printf("PA500::init_act()\n");
	running_act=true;

	device_status=DEVICE_OFF;


	return 0;
}



void PA500alt::execute_sim()
{
	timespec tSleep;
	tSleep.tv_sec=PA500_SLEEP_SEC;
	tSleep.tv_nsec=PA500_SLEEP_NSEC;

	IO_status io_status;



	while(running_sim)
	{
		read_sim_tlm();

		io_status=status->io_status.get();

		if (io_status.tlm.digital[EUROPE_DIO_PA500_1_POWER]==0) device_status=DEVICE_OFF;

		if ((io_status.tlm.digital)[EUROPE_DIO_PA500_1_POWER]==1 && device_status==DEVICE_OFF)
		{
			printf("Init PA500\n");
			usleep(500000);

			device_status=DEVICE_INIT;
		}

		if (io_status.tlm.digital[EUROPE_DIO_PA500_1_POWER]==1 && device_status!=DEVICE_OFF)
		{
			if (range==0.0 || measure_flag==0)
			{
				range=-1.0;
			}
			update_device_status(measure_flag);
		}

		updateStatus();

		nanosleep(&tSleep,NULL);
	}
}


void PA500alt::read_sim_tlm()
{
	PA500_tlm_packet msg;
	int ret;
	measure_flag=0;
	do{
		ret=tlmSim->recv_message((char*)&msg);
		if (ret>0)
		{
			range=(((double)(msg.range))/PA500_factor);
			measure_flag=1;
		}
	}while(ret>0);
}


void PA500alt::update_device_status(int r)
{
	if (r>0)
	{
		missed_receive_count=0;
		device_status=DEVICE_RUNNING;

		if (range==-1.0) device_status=DEVICE_WARNING;
	}

	else if (r <= 0)
	{
		//printf("FOG - ERROR reading message.\n");
		missed_receive_count++;
		if (missed_receive_count>PA500_RECEIVE_WAIT_LOOPS_FOR_WARNING && missed_receive_count<=PA500_RECEIVE_WAIT_LOOPS_FOR_FAULT)
			device_status=DEVICE_WARNING;

		else if (missed_receive_count>PA500_RECEIVE_WAIT_LOOPS_FOR_FAULT)
		{
			device_status=DEVICE_FAULT;
			missed_receive_count=PA500_RECEIVE_WAIT_LOOPS_FOR_FAULT;
		}
	}
}


void PA500alt::execute_act()
{
	timespec tSleep;
	tSleep.tv_sec=PA500_SLEEP_SEC;
	tSleep.tv_nsec=PA500_SLEEP_NSEC;

	IO_status io_status;


	while(running_act)
	{
		io_status=status->io_status.get();

		if (io_status.tlm.digital[EUROPE_DIO_PA500_1_POWER]==0) device_status=DEVICE_OFF;

		if ((io_status.tlm.digital)[EUROPE_DIO_PA500_1_POWER]==1 && device_status==DEVICE_OFF)
		{

			printf("Init PA500\n");
			usleep(500000);

			if(pa500_init(pa500,pa500SerialDeviceName,FREE_RUNNING_PA500_MODE)<0)
			{
				printf("PA500 serial device error\n");
			}

			device_status=DEVICE_INIT;
		}

		if (io_status.tlm.digital[EUROPE_DIO_PA500_1_POWER]==1 && device_status!=DEVICE_OFF)
		{
			if (pa500_read(pa500,&range)==0) measure_flag=1;

			if (range==0.0 || measure_flag==0)
			{
				range=-1.0;
				update_device_status(-1);
			}
			else update_device_status(1);


			updateStatus();

			//send_telemetry();
		}


		nanosleep(&tSleep,NULL);
	}
}




void PA500alt::updateStatus()
{
	Time_status ts;
	ts=status->time_status.get();

	if (missed_receive_count==0) lastValidTimeStamp=ts.timeStamp;

	PA500_status pa500_status;
	pa500_status=status->pa500_status.get();

	if (device_status!=DEVICE_OFF)
	{
		pa500_status.range.value=range;
		pa500_status.timeStamp=lastValidTimeStamp;
	}

	pa500_status.device_status=device_status;
	status->pa500_status.set(pa500_status);

	//printf("range: %lf\n",pa500_status.range);
}


void PA500alt::send_telemetry()
{
	PA500_tlm_packet msg;

	PA500_status pa500_status;
	pa500_status=status->pa500_status.get();

	pa500_status.compose_tlm_packet(msg);

	/*
	msg.range=(int64)(range*PA500_factor);
	msg.device_status=device_status;
	*/

	tlm->send_message((char*)&msg,sizeof(msg));
}


void* start_pa500alt(void *arg)
{
	PA500alt* pThread=(PA500alt*)((ThreadInfo*)arg)->pThread;
	pThread->execute();
	return NULL;
}

