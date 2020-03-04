/*
 * IO_channels.cpp
 *
 *  Created on: Mar 17, 2017
 *      Author: mx
 */


#include "IO_europe.h"


IO_europe::IO_europe(const char *name,NetworkManager &nm, DataAccess<IO_europe_status>& IO_access, DataAccess<DVL_status>& DVL_access,DataAccess<PA500_status>& PA500_access,
	                                                      DataAccess<Echologger_status>& Echologger_access, DataAccess<Pinger_status>& Pinger_access,
	                                                      DataAccess<Time_status>& Time_access): Device(name,SCHED_FIFO,IO_THREAD_PRIORITY,start_io_europe,nm)
{
	strcpy(configFileName,CONFIGURATION_IO_FILE);


	

	state=0;
	tickCount=0;

	time_access = &Time_access;
	io_access = &IO_access;
	dvl_access = &DVL_access;
	pa500_access = &PA500_access;
	echologger_access = &Echologger_access;
	pinger_access = &Pinger_access;
}


IO_europe::~IO_europe()
{

}


int IO_europe::init_sim()
{
	printf("IO_europe::init_sim()\n");

	IO_europe_status io_status;
	io_status = io_access->get();

	for(int i=0;i<EUROPE_DIO_MAX_CHANNELS;i++)
	{
		digitalInput[i]=0;
		digitalOutput[i]=0;
		digitalOutputPulseCounter[i]=0;

		io_status.digitalInput[i] = 0;
		io_status.digitalOutput[i] = 0;
	}

	

	for(int i=0;i<EUROPE_AD_MAX_CHANNELS;i++)
	{
		io_status.analogInput[i] = 0.0;
		analogInput[i]=0.0;
	}

	for(int i=0;i<EUROPE_DA_MAX_CHANNELS;i++)
	{
		io_status.analogOutput[i] = 0.0;
		analogOutput[i]=0.0;
	}

	io_access->set(io_status);

	running_sim=true;

/*
	cmd = new CommLink("IO_cmd", OVERRIDE);
	cmd->open(networkManager->ROBOT_IP, networkManager->IO_ROBOT_HMI_PORT_IN,
			  networkManager->HMI_IP, networkManager->IO_HMI_PORT_OUT);
	cmd->create();
	*/

	cmdSim = new CommLink("IO_cmdSim", UDP_PURE);
	cmdSim->open(networkManager->ROBOT_IP, networkManager->IO_ROBOT_SIM_PORT_OUT,
		      networkManager->SIM_IP, networkManager->IO_SIM_PORT_IN);
	cmdSim->create();


	tlmSim = new CommLink("IO_tlmSim", UDP_PURE);
	tlmSim->open(networkManager->ROBOT_IP, networkManager->IO_ROBOT_SIM_PORT_IN,
		      networkManager->SIM_IP, networkManager->IO_SIM_PORT_OUT);
	tlmSim->create();


	return 0;
}


int IO_europe::init_act()
{
	
	printf("IO_channels::init_act()\n");

	input_output_channels_init(digitalInput,digitalOutput,digitalOutputPulseCounter,analogInput,analogOutput);
	io_boards_init(&diamond0,&diamond1);
	usleep(500000); // wait half a second for allowing devices power up

	running_act=true;

	

	return 0;
}

void IO_europe::execute_sim()
{
	timespec tSleep;
	tSleep.tv_sec=IO_SLEEP_SEC;
	tSleep.tv_nsec=IO_SLEEP_NSEC;

	IO_europe_status io_status;
	

	while(running_sim)
	{
		//read_cmd();
		io_status = io_access->get();
		
		if (io_status.autoStartStop!=0) automaticStartStop();

		send_sim_cmd();

		read_sim_tlm();

		updateStatus();



		//send_telemetry();

		nanosleep(&tSleep,NULL);
	}
}

void IO_europe::execute_act()
{
	timespec tSleep;
	tSleep.tv_sec=IO_SLEEP_SEC;
	tSleep.tv_nsec=IO_SLEEP_NSEC;

	IO_europe_status io_status;
	

	while(running_act)
	{
		//read_cmd();

		if (io_status.autoStartStop != 0) automaticStartStop();

		io_status = io_access->get();
		

		for(int i=0;i<EUROPE_DIO_MAX_CHANNELS;i++)
			digitalOutput[i]= io_status.digitalOutput[i];
		
		for(int i=0;i<EUROPE_DA_MAX_CHANNELS;i++)
			analogOutput[i]= io_status.analogOutput[i];


		


		io_boards_put_analog_outputs(&diamond0,&diamond1,analogOutput);
		io_boards_put_digital_outputs(&diamond0,&diamond1,digitalOutput);

		io_boards_get_analog_inputs(&diamond0,&diamond1,analogInput);
		io_boards_get_digital_inputs(&diamond0,&diamond1,digitalInput);


		for(int i=0;i<EUROPE_DIO_MAX_CHANNELS;i++)
			io_status.digitalInput[i]=digitalInput[i];

		for(int i=0;i<EUROPE_AD_MAX_CHANNELS;i++)
			io_status.analogInput[i]=analogInput[i];

		io_access->set(io_status);

		updateStatus();

		//send_telemetry();
		
		nanosleep(&tSleep,NULL);
	}
}


void IO_europe::read_cmd()
{
	IO_europe_cmd_packet msg;
	int ret;

	IO_europe_status io_status;
	

	do{
		ret=cmd->recv_message((char*)&msg);
		if (ret>0)
		{
			//printf("%d %d %lli\n",msg.cmd_type,msg.index,msg.value);

			io_status = io_access->get();

			switch(msg.cmd_type)
			{
				case SET_SIM:     set_mode(DEVICE_SIM);
				                  break;

				case SET_ACT:     set_mode(DEVICE_ACT);
				                  break;

				case SET_DIGITAL: if (msg.value!=0) msg.value=1;
								  io_status.digitalOutput[msg.index]=msg.value;
								  break;

				case SET_ANALOG:  io_status.analogOutput[msg.index]=((double)msg.value)/IO_factor;
								  break;
/*
				case AUTO_START:  autoStartStop=1;
								  break;

				case AUTO_STOP:   autoStartStop=2;
								  break;*/
			};
			
			
			io_access->set(io_status);
		}

	}while(ret>0);
	
	
}


void IO_europe::send_sim_cmd()
{
	IO_europe_sim_cmd_packet cmd;

	IO_europe_status io_status;
	io_status = io_access->get();

	//printf("send_sim_cmd - digOut[0]: %d    updated: %d\n", io_status.digitalOutput[0],io_status.updated);

	

	cmd.digital = 0;
	for (int i = 0; i < EUROPE_DIO_MAX_CHANNELS; i++)
		cmd.digital = (cmd.digital << 1) + io_status.digitalOutput[EUROPE_DIO_MAX_CHANNELS - 1 - i];



	for (int i = 0; i < EUROPE_DA_MAX_CHANNELS; i++)
		cmd.da[i] = (int64)(io_status.analogOutput[i] * IO_factor);

	//printf("cmd dig: %lli\n", cmd.digital);

	cmdSim->send_message((char*)&cmd, sizeof(cmd));
	
}


void IO_europe::read_sim_tlm()
{
	IO_europe_tlm_packet msg;
	int ret;

	IO_europe_status io_status;

	do{
		ret=tlmSim->recv_message((char*)&msg);
		if (ret>0)
		{
			//printf("tlm dig: %lli\n",msg.digital);

			io_status = io_access->get();

			for(int i=0;i<EUROPE_DIO_MAX_CHANNELS;i++)
			{
				io_status.digitalInput[i]=msg.digital & 1;
				msg.digital=msg.digital>>1;
			}

			for(int i=0;i<EUROPE_AD_MAX_CHANNELS;i++)
				io_status.analogInput[i]=((double)(msg.analogInput[i]))/IO_factor;

			for(int i=0;i<EUROPE_DA_MAX_CHANNELS;i++)
				io_status.analogOutput[i]=((double)(msg.analogOutput[i]))/ IO_factor;

			io_access->set(io_status);
		}

	}while(ret>0);
}


void IO_europe::send_telemetry()
{
	/*
	IO_channels_tlm_packet msg;

	msg.digital=0;
	for(int i=0;i<EUROPE_DIO_MAX_CHANNELS;i++)
		msg.digital=( msg.digital + status->io.tlm.digital[EUROPE_DIO_MAX_CHANNELS-1-i] ) << 1 ;
	msg.digital=msg.digital>>1;

	for(int i=0;i<EUROPE_AD_MAX_CHANNELS;i++)
		msg.ad[i]=(int32)(status->io.tlm.analogInput[i]*IO_channels_factor);

	for(int i=0;i<EUROPE_DA_MAX_CHANNELS;i++)
		msg.da[i]=(int32)(status->io.tlm.analogOutput[i]*IO_channels_factor);

	tlm->send_message((char*)&msg);
	*/
}


void IO_europe::automaticStartStop()
{
	IO_europe_status io_status;
	io_status = io_access->get();
	
	if (state==16)
	{
		io_status.autoStartStop=0;
		state=0;
		tickCount=0;
	}

	else if (io_status.autoStartStop==1)
	{
		if (tickCount>=IO_START_TICKS)
		{
			io_status.digitalOutput[state]=1;
			tickCount=0;
			state++;
		}
		else tickCount++;
	}

	else if (io_status.autoStartStop==2)
	{
		if (tickCount>=IO_STOP_TICKS)
		{
			io_status.digitalOutput[15-state]=0;
			tickCount=0;
			state++;
		}
		else tickCount++;
	}

	io_access->set(io_status);
	
}


void IO_europe::updateStatus()
{
	IO_europe_status io_status;
	io_status = io_access->get();

	DVL_status dvl_status;
	dvl_status = dvl_access->get();
	dvl_status.powered = io_status.digitalInput[EUROPE_DIO_DVL_POWER];
	dvl_access->set(dvl_status);

	PA500_status pa500_status;
	pa500_status = pa500_access->get();
	pa500_status.powered = io_status.digitalInput[EUROPE_DIO_PA500_1_POWER];
	pa500_access->set(pa500_status);

	Echologger_status echologger_status;
	echologger_status = echologger_access->get();
	echologger_status.powered = io_status.digitalInput[EUROPE_DIO_PA500_2_POWER];
	echologger_access->set(echologger_status);

	Pinger_status pinger_status;
	pinger_status = pinger_access->get();
	pinger_status.powered = io_status.digitalInput[EUROPE_DIO_EVOLOGICS_MODEM_POWER];
	pinger_access->set(pinger_status);
}



void* start_io_europe(void *arg)
{
	IO_europe* pThread=(IO_europe*)((ThreadInfo*)arg)->pThread;
	pThread->execute();
	return NULL;
}
