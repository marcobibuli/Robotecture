/*
 * Echologger.cpp
 *
 *  Created on: Feb 05, 2019
 *      Author: mx
 */



#include "Echologger.h"

Echologger::Echologger(const char *name,NetworkManager &nm,Status *s):Device(name,SCHED_FIFO,ECHOLOGGER_THREAD_PRIORITY,start_echologger,nm,s)
{
	strcpy(configFileName,CONFIGURATION_ECHOLOGGER_FILE);


	range=-1.0;
/*
	tlm=new CommLink( "PA500_tlm" , OVERRIDE , sizeof(PA500_tlm_packet) );
	tlm->open( networkManager->ROBOT_IP , networkManager->PA500_ROBOT_PORT_OUT ,
			   networkManager->HMI_IP   , networkManager->PA500_HMI_PORT_IN );
	tlm->create();
*/
	tlmSim=new CommLink( "Micron_tlmSim" , OVERRIDE );
	tlmSim->open( networkManager->ROBOT_IP , networkManager->ECHOLOGGER_SIM_ROBOT_PORT_IN ,
			      networkManager->SIM_IP   , networkManager->ECHOLOGGER_SIM_PORT_OUT );
	tlmSim->create();


	measure_flag=0;
}

Echologger::~Echologger()
{

}


int Echologger::init_sim()
{
	printf("Echologger::init_sim()\n");
	running_sim=true;

	device_status=DEVICE_OFF;

	return 0;
}


int Echologger::init_act()
{
	printf("Echologger::init_act()\n");
	running_act=true;

	device_status=DEVICE_OFF;


	return 0;
}



void Echologger::execute_sim()
{
	timespec tSleep;
	tSleep.tv_sec=ECHOLOGGER_SLEEP_SEC;
	tSleep.tv_nsec=ECHOLOGGER_SLEEP_NSEC;

	IO_status io_status;



	while(running_sim)
	{
		read_sim_tlm();

		io_status=status->io_status.get();

		if (io_status.tlm.digital[EUROPE_DIO_PA500_2_POWER]==0) device_status=DEVICE_OFF;

		if ((io_status.tlm.digital)[EUROPE_DIO_PA500_2_POWER]==1 && device_status==DEVICE_OFF)
		{
			printf("Init Echologger\n");
			usleep(500000);

			device_status=DEVICE_INIT;
		}

		if (io_status.tlm.digital[EUROPE_DIO_PA500_2_POWER]==1 && device_status!=DEVICE_OFF)
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


void Echologger::read_sim_tlm()
{
	Echologger_tlm_packet msg;
	int ret;
	measure_flag=0;
	do{
		ret=tlmSim->recv_message((char*)&msg);
		if (ret>0)
		{
			range=(((double)(msg.range))/Echologger_factor);
			measure_flag=1;
		}
	}while(ret>0);
}


void Echologger::update_device_status(int r)
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
		if (missed_receive_count>ECHOLOGGER_RECEIVE_WAIT_LOOPS_FOR_WARNING && missed_receive_count<=ECHOLOGGER_RECEIVE_WAIT_LOOPS_FOR_FAULT)
			device_status=DEVICE_WARNING;

		else if (missed_receive_count>ECHOLOGGER_RECEIVE_WAIT_LOOPS_FOR_FAULT)
		{
			device_status=DEVICE_FAULT;
			missed_receive_count=ECHOLOGGER_RECEIVE_WAIT_LOOPS_FOR_FAULT;
		}
	}
}


void Echologger::execute_act()
{
	timespec tSleep;
	tSleep.tv_sec=ECHOLOGGER_SLEEP_SEC;
	tSleep.tv_nsec=ECHOLOGGER_SLEEP_NSEC;

	IO_status io_status;


	while(running_act)
	{/*
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
	*/

		nanosleep(&tSleep,NULL);
	}
}




void Echologger::updateStatus()
{
	Time_status ts;
	ts=status->time_status.get();

	if (missed_receive_count==0) lastValidTimeStamp=ts.timeStamp;

	Echologger_status echologger_status;
	echologger_status=status->echologger_status.get();

	if (device_status!=DEVICE_OFF)
	{
		echologger_status.range.value=range;
		echologger_status.timeStamp=lastValidTimeStamp;
	}

	echologger_status.device_status=device_status;
	status->echologger_status.set(echologger_status);

	//printf("status: %d     range: %lf\n",echologger_status.device_status,echologger_status.range.value);
}


void Echologger::send_telemetry()
{
	Echologger_tlm_packet msg;

	Echologger_status echologger_status;
	echologger_status=status->echologger_status.get();

	echologger_status.compose_tlm_packet(msg);

	/*
	msg.range=(int64)(range*PA500_factor);
	msg.device_status=device_status;
	*/

	tlm->send_message((char*)&msg,sizeof(msg));
}


void* start_echologger(void *arg)
{
	Echologger* pThread=(Echologger*)((ThreadInfo*)arg)->pThread;
	pThread->execute();
	return NULL;
}

