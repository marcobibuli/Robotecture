/*
 * DVL.cpp
 *
 *  Created on: Jul 24, 2018
 *      Author: mx
 */



#include "DVL.h"

DVL::DVL(const char *name,NetworkManager &nm,DataAccess<DVL_status>& DVL_access, DataAccess<IO_europe_status>& IO_access, DataAccess<Time_status>& Time_access):Device(name,SCHED_FIFO,DVL_THREAD_PRIORITY,start_dvl,nm)
{
	strcpy(configFileName,CONFIGURATION_DVL_FILE);
	time_access = &Time_access;
	dvl_access = &DVL_access;
	io_access = &IO_access;
/*
	tlm=new CommLink( "DVL_tlm" , OVERRIDE , sizeof(DVL_tlm_packet) );
	tlm->open( networkManager->ROBOT_IP , networkManager->DVL_ROBOT_PORT_OUT ,
			   networkManager->HMI_IP   , networkManager->DVL_HMI_PORT_IN );
	tlm->create();
*/

	

	packetLength = 326;

	range1=0.0; 	range2=0.0; 	range3=0.0; 	range4=0.0;
	altitude=0.0;
	surgeVelocity=0.0;  swayVelocity=0.0;  heaveVelocity=0.0;  errorVelocity=0.0;
	validVelocity=0;

	surge_int=0;  sway_int=0;  heave_int=0;
    su=0.0;  sv=0.0;

    m_pSio=NULL;

    measure_flag=0;
}

DVL::~DVL()
{

}


int DVL::init_sim()
{
	printf("DVL::init_sim()\n");
	running_sim=true;

	device_status=DEVICE_OFF;

	tlmSim = new CommLink("DVL_tlmSim", UDP_PURE);
	tlmSim->open(networkManager->ROBOT_IP, networkManager->DVL_ROBOT_SIM_PORT_IN,
		networkManager->SIM_IP, networkManager->DVL_SIM_PORT_OUT);
	tlmSim->create();

	return 0;
}


int DVL::init_act()
{
	printf("DVL::init_act()\n");
	running_act=true;

	strcpy(dvlexplorerSerialDeviceName,dvl_serial_name);
	m_pSio=new Sio();

	m_pSio->init(dvlexplorerSerialDeviceName, //ttyUSB0
			B9600,
			CS8,
			0,
			0,
			0,
			CREAD,
			0);

	device_status=DEVICE_OFF;

	return 0;
}



void DVL::execute_sim()
{
	timespec tSleep;
	tSleep.tv_sec=DVL_SLEEP_SEC;
	tSleep.tv_nsec=DVL_SLEEP_NSEC;

	IO_europe_status io_status;

	while(running_sim)
	{
		read_sim_tlm();

		io_status = io_access->get();

		//printf("*** %d   %d\n", io_status.digitalInput[EUROPE_DIO_DVL_POWER], device_status);

		if (io_status.digitalInput[EUROPE_DIO_DVL_POWER]==0) device_status=DEVICE_OFF;

		if (io_status.digitalInput[EUROPE_DIO_DVL_POWER]==1 && device_status==DEVICE_OFF)
		{
			printf("Init DVL\n");
			usleep(500000);

			device_status=DEVICE_INIT;
		}

		if (io_status.digitalInput[EUROPE_DIO_DVL_POWER]==1 && device_status!=DEVICE_OFF)
		{
			update_device_status(measure_flag);
		}

		updateStatus();

		nanosleep(&tSleep,NULL);
	}
}


void DVL::read_sim_tlm()
{
	DVL_tlm_packet msg;
	int ret;
	measure_flag=0;
	do{
		ret=tlmSim->recv_message((char*)&msg);
		if (ret>0)
		{
			range1=(((double)(msg.range1))/DVL_factor);
			range2=(((double)(msg.range2))/DVL_factor);
			range3=(((double)(msg.range3))/DVL_factor);
			range4=(((double)(msg.range4))/DVL_factor);
			surgeVelocity=(((double)(msg.surgeVelocity))/DVL_factor);
			swayVelocity=(((double)(msg.swayVelocity))/DVL_factor);
			heaveVelocity=(((double)(msg.heaveVelocity))/DVL_factor);
			errorVelocity=(((double)(msg.errorVelocity))/DVL_factor);
			validVelocity=(int)(msg.validVelocity);

			altitude=(range1+range2+range3+range4)*cos(RANGE_ANGLE)/4.0;

			measure_flag=1;

			update_device_status(ret);

			printf("*** altitude: %lf   surgeVelocity: %lf   swayVelocity: %lf   valid: %d\n", altitude, surgeVelocity, swayVelocity, validVelocity);

		}

	}while(ret>0);
}


void DVL::update_device_status(int r)
{
	if (r>0)
	{
		missed_receive_count=0;
		device_status=DEVICE_RUNNING;

		if (validVelocity==0) device_status=DEVICE_WARNING;
	}

	else if (r <= 0)
	{
		//printf("FOG - ERROR reading message.\n");
		missed_receive_count++;
		if (missed_receive_count>DVL_RECEIVE_WAIT_LOOPS_FOR_WARNING && missed_receive_count<=DVL_RECEIVE_WAIT_LOOPS_FOR_FAULT)
			device_status=DEVICE_WARNING;

		else if (missed_receive_count>DVL_RECEIVE_WAIT_LOOPS_FOR_FAULT)
		{
			device_status=DEVICE_FAULT;
			missed_receive_count=DVL_RECEIVE_WAIT_LOOPS_FOR_FAULT;
		}
	}
}



void DVL::execute_act()
{
	timespec tSleep;
	tSleep.tv_sec=DVL_SLEEP_SEC;
	tSleep.tv_nsec=DVL_SLEEP_NSEC;

	int received=0;

	unsigned char *message = new unsigned char[packetLength];

	IO_europe_status io_status;


	while(running_act)
	{
		io_status=io_access->get();

		if (io_status.digitalInput[EUROPE_DIO_DVL_POWER]==0) device_status=DEVICE_OFF;

		if (io_status.digitalInput[EUROPE_DIO_DVL_POWER]==1 && device_status==DEVICE_OFF)
		{
			printf("Init DVL\n");
			usleep(500000);
			initExplorerDVL();

			device_status=DEVICE_INIT;
		}

		if (io_status.digitalInput[EUROPE_DIO_DVL_POWER]==1 && device_status!=DEVICE_OFF)
		{
			received = this->receive(message);

			//if (received < 0) printf("DVL ERROR reading message\n");

			update_device_status(received);
		}

		updateStatus();


		nanosleep(&tSleep,NULL);
	}
}


void DVL::updateStatus()
{
	Time_status ts;
	ts=time_access->get();

	DVL_status dvl_status;
	dvl_status=dvl_access->get();

	if (device_status!=DEVICE_OFF)
	{
		dvl_status.range1.value=range1;
		dvl_status.range2.value=range2;
		dvl_status.range3.value=range3;
		dvl_status.range4.value=range4;
		dvl_status.altitude.value=altitude;
		dvl_status.surgeVelocity.value=surgeVelocity;
		dvl_status.swayVelocity.value=swayVelocity;
		dvl_status.heaveVelocity.value=heaveVelocity;
		dvl_status.errorVelocity.value=errorVelocity;
		dvl_status.validVelocity=validVelocity;

		dvl_status.timeStamp=ts.timeStamp;
	}

	dvl_status.device_status=device_status;
	
	dvl_access->set(dvl_status);

	//printf("altitude: %lf   surgeVelocity: %lf   swayVelocity: %lf   valid: %d\n",dvl_status.altitude,dvl_status.surgeVelocity,dvl_status.swayVelocity,dvl_status.validVelocity);

}


void DVL::send_telemetry()
{
	DVL_tlm_packet msg;

	DVL_status dvl_status;
	dvl_status = dvl_access->get();

	dvl_status.compose_tlm_packet(msg);

	/*
	msg.range1=(int64)((range1)*DVL_factor);
	msg.range2=(int64)((range2)*DVL_factor);
	msg.range3=(int64)((range3)*DVL_factor);
	msg.range4=(int64)((range4)*DVL_factor);
	msg.altitude=(int64)((altitude)*DVL_factor);
	msg.surgeVelocity=(int64)((surgeVelocity)*DVL_factor);
	msg.swayVelocity=(int64)((swayVelocity)*DVL_factor);
	msg.heaveVelocity=(int64)((heaveVelocity)*DVL_factor);
	msg.errorVelocity=(int64)((errorVelocity)*DVL_factor);
	msg.validVelocity=(int64)((validVelocity)*DVL_factor);
	msg.device_status=device_status;
	*/

	tlm->send_message((char*)&msg,sizeof(msg));
}


int DVL::initExplorerDVL()
{
	timespec tDelay;
	tDelay.tv_sec=0;
	tDelay.tv_nsec=300000000;

	ssize_t written = 0, read = 0;
	char received[MAXIMUM_RECEIVED];

	// Send the BREAK command to the device.
	//printf("BREAK\n");
	written = this->m_pSio->write("===", 3);
	//printf("BREAK\n");
	if (3 != written)
	{
		printf("Error: BREAK Expected to write 3 bytes but only wrote %d\n",written);
		return -1;
	}

	nanosleep(&tDelay, NULL);


	printf("Init before read\n");
	read = this->m_pSio->read(received, MAXIMUM_RECEIVED);
	printf("Init after read %d\n",read);
	if (read == 0)
	{
		printf("Error: Unable to read DVL header from serial port\n");
		return -2;
	}
	for(int i=0; i<read; i++)
		printf("%d",received[i]);
	printf("\n");


	// Send the CR1 command to the device.
	//printf("CR1\n");
	written = this->m_pSio->write("CR1\r", 4);
	//printf("CR1\n");
	if (4 != written)
	{
		printf("Error: CR1 Expected to write 4 bytes but only wrote %d\n",written);
		return -1;
	}

	nanosleep(&tDelay, NULL);


	read = this->m_pSio->read(received, MAXIMUM_RECEIVED);
	if (read == 0)
	{
		printf("Error: Unable to read DVL header from serial port\n");
		return -2;
	}
	for(int i=0; i<read; i++)
		printf("%d",received[i]);
	printf("\n");


	// Send the CB411 command to the device.
	//printf("CB411\n");
	written = this->m_pSio->write("CB411\r", 6);
	//printf("CB411\n");
	if (6 != written)
	{
		printf("Error: CB411 Expected to write 6 bytes but only wrote %d\n",written);
		return -1;
	}

	nanosleep(&tDelay, NULL);

	read = this->m_pSio->read(received, MAXIMUM_RECEIVED);
	if (read == 0)
	{
		printf("Error: Unable to read DVL header from serial port\n");
		return -2;
	}
	for(int i=0; i<read; i++)
		printf("%d",received[i]);
	printf("\n");


	// Send the CF11110 command to the device.
#ifdef SINGLE_PING_MODE

		//printf("CF00110\n");
			written = this->m_pSio->write("CF00110\r", 8);
			//printf("CF00110\n");
			if (8 != written)
			{
				printf("Error: CF11110 Expected to write 8 bytes but only wrote %d\n", written);
				return -1;
			}

			nanosleep(&tDelay, NULL);


			read = this->m_pSio->read(received, MAXIMUM_RECEIVED);
			if (read == 0)
			{
				printf(""Error: Unable to read DVL header from serial port" << endl;
				return -2;
			}
			for(int i=0; i<read; i++)
				printf("received[i];
			printf("\n");
#else
		//printf("CF11110\n");
			written = this->m_pSio->write("CF11110\r", 8);
			//printf("CF11110\n");
			if (8 != written)
			{
				printf("Error: CF11110 Expected to write 8 bytes but only wrote %d\n",written);
				return -1;
			}

			nanosleep(&tDelay, NULL);


			read = this->m_pSio->read(received, MAXIMUM_RECEIVED);
			if (read == 0)
			{
				printf("Error: Unable to read DVL header from serial port\n");
				return -2;
			}
			for(int i=0; i<read; i++)
				printf("%d",received[i]);
			printf("\n");
#endif



	// Send the BP001 command to the device.
	//printf("BP001\n");
	written = this->m_pSio->write("BP001\r", 6);
	//printf("BP001\n");
	if (6 != written)
	{
		printf("Error: BP001 Expected to write 6 bytes but only wrote %d\n",written);
		return -1;
	}

	nanosleep(&tDelay, NULL);


	read = this->m_pSio->read(received, MAXIMUM_RECEIVED);
	if (read == 0)
	{
		printf("Error: Unable to read DVL header from serial port\n");
		return -2;
	}
	for(int i=0; i<read; i++)
		printf("%d",received[i]);
	printf("\n");



	// Send the BX50(BX01000) command to the device.
	//printf("BX50\n");
	written = this->m_pSio->write("BX50\r", 5);
	//printf("BX50\n");
	if (5 != written)
	{
		printf("Error: BX50 Expected to write 5 bytes but only wrote %d\n",written);
		return -1;
	}

	nanosleep(&tDelay, NULL);


	read = this->m_pSio->read(received, MAXIMUM_RECEIVED);
	if (read == 0)
	{
		printf("Error: Unable to read DVL header from serial port\n");
		return -2;
	}
	for(int i=0; i<read; i++)
		printf("%d",received[i]);
	printf("\n");



	// Send the EA00000 command to the device.
	//printf("EA00000\n");
	written = this->m_pSio->write("EA00000\r", 8);
	//printf("EA00000\n");
	if (8 != written)
	{
		printf("Error: EA00000 Expected to write 8 bytes but only wrote %d\n",written);
		return -1;
	}

	nanosleep(&tDelay, NULL);


	read = this->m_pSio->read(received, MAXIMUM_RECEIVED);
	if (read == 0)
	{
		printf("Error: Unable to read DVL header from serial port\n");
		return -2;
	}
	for(int i=0; i<read; i++)
		printf("%d",received[i]);
	printf("\n");


	// Send the ED0000 command to the device.
	//printf("ED0000\n");
	written = this->m_pSio->write("ED0000\r", 7);
	//printf("ED0000\n");
	if (7 != written)
	{
		printf("Error: ED0000 Expected to write 7 bytes but only wrote %d\n",written);
		return -1;
	}

	nanosleep(&tDelay, NULL);


	read = this->m_pSio->read(received, MAXIMUM_RECEIVED);
	if (read == 0)
	{
		printf("Error: Unable to read DVL header from serial port\n");
		return -2;
	}
	for(int i=0; i<read; i++)
		printf("%d",received[i]);
	printf("\n");



	// Send the ES35 command to the device.
	//printf("ES35\n");
	written = this->m_pSio->write("ES35\r", 5);
	//printf("ES35\n");
	if (5 != written)
	{
		printf("Error: ES35 Expected to write 5 bytes but only wrote %d\n",written);
		return -1;
	}

	nanosleep(&tDelay, NULL);


	read = this->m_pSio->read(received, MAXIMUM_RECEIVED);
	if (read == 0)
	{
		printf("Error: Unable to read DVL header from serial port\n");
		return -2;
	}
	for(int i=0; i<read; i++)
		printf("%d",received[i]);
	printf("\n");


	// Send the EX01111 command to the device. Ship coordinates
	//printf("EX01111\n");
	//written = this->m_pSio->write("EX01000\r", 8);//Instrument coord
	written = this->m_pSio->write("EX01111\r", 8);//TBC
	//printf("EX01111\n");
	if (8 != written)
	{
		printf("Error: EX01111 Expected to write 8 bytes but only wrote %d\n",written);
		return -1;
	}

	nanosleep(&tDelay, NULL);


	read = this->m_pSio->read(received, MAXIMUM_RECEIVED);
	if (read == 0)
	{
		printf("Error: Unable to read DVL header from serial port\n");
		return -2;
	}
	for(int i=0; i<read; i++)
		printf("%d",received[i]);
	printf("\n");


	// Send the EZ11211210(EZ22222220) command to the device.
	//printf("EZ11211210\n");
	written = this->m_pSio->write("EZ11211210\r", 11);
	//printf("EZ11211210\n");
	if (11 != written)
	{
		printf("Error: EZ11211210 Expected to write 11 bytes but only wrote %d\n",written);
		return -1;
	}

	nanosleep(&tDelay, NULL);


	read = this->m_pSio->read(received, MAXIMUM_RECEIVED);
	if (read == 0)
	{
		printf("Error: Unable to read DVL header from serial port\n");
		return -2;
	}
	for(int i=0; i<read; i++)
		printf("%d",received[i]);
	printf("\n");



	// Send the TE00000000 command to the device.
	//printf("TE00000000\n");
	written = this->m_pSio->write("TE00000000\r", 11);
	//printf("TE00000000\n");
	if (11 != written)
	{
		printf("Error: TE00000000 Expected to write 11 bytes but only wrote %d\n",written);
		return -1;
	}

	nanosleep(&tDelay, NULL);


	read = this->m_pSio->read(received, MAXIMUM_RECEIVED);
	if (read == 0)
	{
		printf("Error: Unable to read DVL header from serial port\n");
		return -2;
	}
	for(int i=0; i<read; i++)
		printf("%d",received[i]);
	printf("\n");



	// Send the TP00:00.00 command to the device.
	//printf("TP00:00.00\n");
	written = this->m_pSio->write("TP00:00.00\r", 11);
	//printf("TP00:00.00\n");
	if (11 != written)
	{
		printf("Error: TP00:00.00 Expected to write 11 bytes but only wrote %d\n",written);
		return -1;
	}

	nanosleep(&tDelay, NULL);


	read = this->m_pSio->read(received, MAXIMUM_RECEIVED);
	if (read == 0)
	{
		printf("Error: Unable to read DVL header from serial port\n");
		return -2;
	}
	for(int i=0; i<read; i++)
		printf("%d",received[i]);
	printf("\n");


	// Send the #BF00028 command to the device.
	//printf("#BF00028\n");
	written = this->m_pSio->write("#BF00028\r", 9);
	//printf("#BF00028\n");
	if (9 != written)
	{
		printf("Error: #BF00028 Expected to write 9 bytes but only wrote %d\n",written);
		return -1;
	}

	nanosleep(&tDelay, NULL);


	read = this->m_pSio->read(received, MAXIMUM_RECEIVED);
	if (read == 0)
	{
		printf("Error: Unable to read DVL header from serial port\n");
		return -2;
	}
	for(int i=0; i<read; i++)
		printf("%d",received[i]);
	printf("\n");


	////Water profiling feature - non present
	//EVENTUALLY, INITIALIZE HERE THE WATER PROFILING
	////Water profiling feature - non present

	// Send the #BK0 command to the device.
	//printf("#BK0\n");
	written = this->m_pSio->write("#BK0\r", 5);
	//printf("#BK0\n");
	if (5 != written)
	{
		printf("Error: #BK0 Expected to write 5 bytes but only wrote %d\n",written);
		return -1;
	}

	nanosleep(&tDelay, NULL);


	read = this->m_pSio->read(received, MAXIMUM_RECEIVED);
	if (read == 0)
	{
		printf("Error: Unable to read DVL header from serial port\n");
		return -2;
	}
	for(int i=0; i<read; i++)
		printf("%d",received[i]);
	printf("\n");



	// Send the #BL20,80,160 command to the device.
	//printf("#BL20,80,160\n");
	written = this->m_pSio->write("#BL20,80,160\r", 13);
	//printf("#BL20,80,160\n");
	if (13 != written)
	{
		printf("Error: #BL20,80,160 Expected to write 13 bytes but only wrote %d\n",written);
		return -1;
	}

	nanosleep(&tDelay, NULL);


	read = this->m_pSio->read(received, MAXIMUM_RECEIVED);
	if (read == 0)
	{
		printf("Error: Unable to read DVL header from serial port\n");
		return -2;
	}
	for(int i=0; i<read; i++)
		printf("%d",received[i]);
	printf("\n");




	// Send the #EE0000111 command to the device.
	//printf("#EE0000111\n");
	written = this->m_pSio->write("#EE0000111\r", 11);
	//printf("#EE0000111\n");
	if (11 != written)
	{
		printf("Error: #EE0000111 Expected to write 11 bytes but only wrote %d\n",written);
		return -1;
	}

	nanosleep(&tDelay, NULL);


	read = this->m_pSio->read(received, MAXIMUM_RECEIVED);
	if (read == 0)
	{
		printf("Error: Unable to read DVL header from serial port\n");
		return -2;
	}
	for(int i=0; i<read; i++)
		printf("%d",received[i]);
	printf("\n");



	// Send the #EV0 command to the device.
	//printf("#EV0\n");
	written = this->m_pSio->write("#EV0\r", 5);
	//printf("#EV0\n");
	if (5 != written)
	{
		printf("Error: #EV0 Expected to write 5 bytes but only wrote %d\n",written);
		return -1;
	}

	nanosleep(&tDelay, NULL);


	read = this->m_pSio->read(received, MAXIMUM_RECEIVED);
	if (read == 0)
	{
		printf("Error: Unable to read DVL header from serial port\n");
		return -2;
	}
	for(int i=0; i<read; i++)
		printf("%d",received[i]);
	printf("\n");


	// Send the #EY 0 0 0 0 0 0 0 0 command to the device.
	//printf("#EY 0 0 0 0 0 0 0 0\n");
	written = this->m_pSio->write("#EY 0 0 0 0 0 0 0 0\r", 20);
	//printf("#EY 0 0 0 0 0 0 0 0\n");
	if (20 != written)
	{
		printf("Error: #EY 0 0 0 0 0 0 0 0 Expected to write 20 bytes but only wrote %d\n",written);
		return -1;
	}

	nanosleep(&tDelay, NULL);


	read = this->m_pSio->read(received, MAXIMUM_RECEIVED);
	if (read == 0)
	{
		printf("Error: Unable to read DVL header from serial port %d\n",written);
		return -2;
	}
	for(int i=0; i<read; i++)
		printf("%d",received[i]);
	printf("\n");


	//********************
	// Send the #BI010 command to the device.
	//printf("#BI010\n");
	written = this->m_pSio->write("#BI010\r", 7);
	//printf("#BI010\n");
	if (7 != written)
	{
		printf("Error: #BI010 Expected to write 7 bytes but only wrote %d\n",written);
		return -1;
	}

	nanosleep(&tDelay, NULL);


	read = this->m_pSio->read(received, MAXIMUM_RECEIVED);
	if (read == 0)
	{
		printf("Error: Unable to read DVL header from serial port\n");
		return -2;
	}
	for(int i=0; i<read; i++)
		printf("%d",received[i]);
	printf("\n");
	//********************+


	// Send the #PD0 command to the device.
	//printf("#PD0\n");
	written = this->m_pSio->write("#PD0\r", 5);
	//printf("#PD0\n");
	if (5 != written)
	{
		printf("Error: #PD0 Expected to write 5 bytes but only wrote %d\n",written);
		return -1;
	}

	nanosleep(&tDelay, NULL);


	read = this->m_pSio->read(received, MAXIMUM_RECEIVED);
	if (read == 0)
	{
		printf("Error: Unable to read DVL header from serial port\n");
		return -2;
	}
	for(int i=0; i<read; i++)
		printf("%d",received[i]);
	printf("\n");



	// Send the #CT1 command to the device.
	//printf("#CT1\n");
	written = this->m_pSio->write("#CT1\r", 5);
	//printf("#CT1\n");
	if (5 != written)
	{
		printf("Error: #CT1 Expected to write 5 bytes but only wrote %d\n",written);
		return -1;
	}

	nanosleep(&tDelay, NULL);


	read = this->m_pSio->read(received, MAXIMUM_RECEIVED);
	if (read == 0)
	{
		printf("Error: Unable to read DVL header from serial port\n");
		return -2;
	}
	for(int i=0; i<read; i++)
		printf("%d",received[i]);
	printf("\n");


	// Send the CK command to the device.
	//printf("CK\n");
	written = this->m_pSio->write("CK\r", 3);
	//printf("CK\n");
	if (3 != written)
	{
		printf("Error: CK Expected to write 3 bytes but only wrote %d\n",written);
		return -1;
	}

	nanosleep(&tDelay, NULL);


	read = this->m_pSio->read(received, MAXIMUM_RECEIVED);
	if (read == 0)
	{
		printf("Error: Unable to read DVL header from serial port\n");
		return -2;
	}
	for(int i=0; i<read; i++)
		printf("%d",received[i]);
	printf("\n");


#ifndef SINGLE_PING_MODE
	// Send the CS command to the device.
	//printf("CS\n");
	written = this->m_pSio->write("CS\r", 3);
	//printf("CS\n");
	if (3 != written)
	{
		printf("Error: CS Expected to write 3 bytes but only wrote %d\n",written);
		return -1;
	}

	nanosleep(&tDelay, NULL);


	read = this->m_pSio->read(received, MAXIMUM_RECEIVED);
	if (read == 0)
	{
		printf("Error: Unable to read DVL header from serial port\n");
		return -2;
	}
	for(int i=0; i<read; i++)
		printf("%d",received[i]);
	printf("\n");

	printf("Init complete\n");
#endif

	return 0;
}





int DVL::receive(unsigned char *recMessage)
{
	//dprintf("DVLExplorer::receive(char *)\n");

	ssize_t bytesTransferred = 0;

#ifdef SINGLE_PING_MODE
	unsigned char ready2ping;
	bool ready;
	unsigned char DVLpingPck_tot[this->packetLength+6]; //At the end of the single packet DVL will return also "enter" and "<" chars
	unsigned char DVLpingPck[this->packetLength];
	unsigned char fuffaPckt[MAXIMUM_RECEIVED];
	unsigned char enterCmd[2];
	//int read_size, write_size;
	ssize_t read_size, write_size;
	char recvbuf[DEFAULT_BUFLEN];
	unsigned char pingReady[5];//[6];
#else

	//bool success;
	unsigned char header;
	unsigned char dummyData[2];

	int numByte = 0;
	unsigned char DVLpacket[this->packetLength-4]; //tolgo l'header (2 Byte) e il numByte (2 Byte) letto in dummyData
	unsigned char DVLpacket_tot[this->packetLength];
#endif



#ifdef SINGLE_PING_MODE
	//SINGLE PING MODE
	enterCmd[0] = 0x0D;
	enterCmd[1] = 0x0A;


	//Listen
	listen(this->m_socket_desc , 3);

	//Accept and incoming connection
	puts("Waiting for incoming connections...");
	int c = sizeof(struct sockaddr_in);

	//accept connection from an incoming client
	this->m_client_sock = accept(this->m_socket_desc, (struct sockaddr *)&(this->m_client), (socklen_t*)&c);
	if (this->m_client_sock < 0)
	{
		perror("accept failed");
		return 1;
	}
	puts("Connection accepted");

	//Receive a message from client
	/*do
	{
		read_size = recv(this->m_client_sock , recvbuf , 2000 , 0);
	}
	while(read_size < DEFAULT_BUFLEN);*/
	read_size = recv(this->m_client_sock , recvbuf , 2000 , 0);

	//fprintf(stderr, "ARRIVATO: %s mandati: %d\n", recvbuf, read_size);

	std::printf(""read_size: " << read_size << " - recvbuf: " << recvbuf << " - strlen(recvbuf): " << strlen(recvbuf) << std::endl;


	if( (read_size > 0) && (strncmp(recvbuf, "PING", DEFAULT_BUFLEN) == 0) )
	{
		//PROVA MIA 5 APRILE

		m_pSio->flush();
		write_size = this->m_pSio->write("CS\r", 3);
		//printf("CS\n");
		if (3 != write_size)
		{
			printf(""Error: CS Expected to write 3 bytes but only wrote" << write_size << endl;

			// Close the just accepted socket to free resources
			close(this->m_client_sock);

			return -1;
		}

		//read_size = this->m_pSio->read(pingReady, sizeof(pingReady));
		//read_size = this->m_pSio->read(fuffaPckt, sizeof(MAXIMUM_RECEIVED));

		do{
			read_size = this->m_pSio->read(fuffaPckt, 1);
			//fprintf(stderr, "read_size: %d fuffaPckt: %02X\n", read_size, fuffaPckt[0]);
		}
		while(fuffaPckt[0] != '<');
		//fprintf(stderr, "USCITA. fuffaPckt: %02X\n", fuffaPckt);

		/*
		fprintf(stderr, "pingReady: ");
		for(int i=0; i<sizeof(pingReady); i++)
			fprintf(stderr, "%X", pingReady[i]);
		fprintf(stderr, "\n");
		 */

		//if(pingReady[read_size-1] == '<')
		//if(fuffaPckt[read_size-1] == '<')
		{

			//Send an <Enter> to retrieve the ping
			//fprintf(stderr, "Sending the Enter command to retrieve the ping...\n");

			write_size = this->m_pSio->write(enterCmd, 2);
			//printf("Enter cmd\n");
			if (2 != write_size)
			{
				printf(""Error: Enter cmd Expected to write 2 bytes but only wrote" << write_size << endl;

				// Close the just accepted socket to free resources
				close(this->m_client_sock);

				return -1;
			}
			//fprintf(stderr, "Sent the Enter command to retrieve the ping.\n");


			std::printf(""Inizia la read..." << std::endl;

			int acc = 0;
			do{
				read_size = this->m_pSio->read(fuffaPckt, this->packetLength+6-acc);
				for(int i=acc; i<acc+read_size; i++)
					DVLpingPck_tot[i] = fuffaPckt[i-acc];
				acc += read_size;
				//fprintf(stderr, "LETTI: %d\n", acc);
			}
			while(acc < this->packetLength+6);

			std::printf(""Finita la read..." << std::endl;

			//////*************************************************************************************************************+++
			//if( (DVLpingPck_tot[0] != 0x7F) || (DVLpingPck_tot[1] != 0x7F) )
				//fprintf(stderr, "Expected symbol not found\n");//return FALSE;
			//else
			//	fprintf(stderr, "Expected symbol found\n");//return FALSE;

			for(int i=0; i<this->packetLength; i++)
				DVLpingPck[i] = DVLpingPck_tot[i];


			std::printf(""Finita la copia..." << std::endl;

			uint16 receivedChsm = (DVLpingPck[this->packetLength-2] << 8) + DVLpingPck[this->packetLength-1];
			unsigned char b1, b2;
			uint16 computedChksm = this->computeChecksum(DVLpingPck_tot, this->packetLength-2, b1, b2);
		//	fprintf(stderr, "%d  VS  %d    0x%02X 0x%02X     0x%02X 0x%02X\n", receivedChsm, computedChksm, b1, b2, DVLpingPck[this->packetLength-2], DVLpingPck[this->packetLength-1]);
			//std::printf(""" << receivedChsm << "   VS  " << computedChksm << " 0x" << b1 << "  0x" << b2 << " 0x" <<  DVLpingPck[PACKET_SIZE-6] << " 0x" << DVLpingPck[PACKET_SIZE-5] << std::endl;
			if(receivedChsm == computedChksm)
			{
				//fprintf(pippo, "*************Ok Checksum!\n");
				//fprintf(stderr, "*************Ok Checksum!\n");
				this->convertData(DVLpingPck);//Occhio perchè DVLpingPcl_tot è PACKET_SIZE+3!!!
				//this->printReceivedPck(DVLpingPck_tot, this->packetLength+6);//Occhio perchè DVLpingPcl_tot è PACKET_SIZE+3!!!

				// Echo the buffer back to the sender
				strcpy (recvbuf, "DONE\n");
				write_size = write(this->m_client_sock , recvbuf , strlen(recvbuf));
				if (write_size == -1)
				{
					printf("send failed\n");
					close(this->m_client_sock);
					return 1;
				}

				//printf("Bytes sent: %d\n", write_size);
				//return true;
			}
			else
			{
				//fprintf(pippo, "+++++++++++++No Checksum!\n");
				//fprintf(stderr, "+++++++++++++No Checksum!\n");
				//this->printReceivedPck(DVLpingPck_tot, this->packetLength+6);
				//return false;

				// Echo the buffer back to the sender
				strcpy (recvbuf, "DONE\n");
				write_size = write(this->m_client_sock , recvbuf , strlen(recvbuf));
				if (write_size == -1)
				{
					printf("send failed\n");
					close(this->m_client_sock);
					return 1;
				}
			}

			// Close the just accepted socket to free resources
			close(this->m_client_sock);

		}
		/*else
			{
				fprintf(stderr, "Unable to ping...\n");
				return -5;
			}*/
		//PROVA MIA 5 APRILE

	}
	else if (read_size == 0)
		printf("Received nothing...\n");
	else if (strncmp(recvbuf, "PING", DEFAULT_BUFLEN) != 0)
		printf("Received: %s...\n", recvbuf);
	else
	{
		printf("recv failed\n");
		close(this->m_client_sock);
		return 1;
	}

	//PROVA MIA 5 APRILE

	return 0;
	//SINGLE PING MODE
#else
	do
	{
		//printf("Before read\n");
		// Read the first 4 bytes to distinguish AHRS/GPS packet
		bytesTransferred = m_pSio->read(&header, sizeof(header));
		//out << "header: " << hex << header << endl;
		//printf("header: 0x%02X\n", header);
		//fprintf(this->myfile, "header: 0x2%X\n", header);
		//printf("E qui ho scritto il file\n");
		//printf("After read %d\n",bytesTransferred);
		if (sizeof(header) != bytesTransferred)
		{
			//printf(""1. Error: Expected to read " << sizeof(header) << " bytes but only read " << bytesTransferred << " (timeout). Check the DVL communication parameters\n" << endl;
			return -1;
		}
	}
	while(header != 0x7F);
	//printf("+++++++++++++++%02X\n", header);

	bytesTransferred = m_pSio->read(&header, sizeof(header));
	if (sizeof(header) != bytesTransferred)
	{
		//printf(""2. Error: Expected to read " << sizeof(header) << " bytes but only read " << bytesTransferred << " (timeout). Check the DVL communication parameters\n" << endl;
		return -1;
	}
	//printf("***************%02X\n", header);
	if(header == 0x7F)
	{
		bytesTransferred =  m_pSio->read(dummyData, sizeof(dummyData));
		if (sizeof(dummyData) != bytesTransferred)
		{
			//printf(""3. Error: Expected to read " << sizeof(header) << " bytes but only read " << bytesTransferred << " (timeout). Check the DVL communication parameters\n" << endl;
			return -1;
		}
	}
	else
	{
		//printf(""Desynchronized DVL packet" << endl;
		return -1;
	}

	//printf(""BYTES: 0x"<<  dummyData[1] << "  0x" << dummyData[0];


	numByte = ((dummyData[1] << 8) + dummyData[0]) - 2;

	//printf(""BYTES: " << numByte << endl;

	if (numByte<=MAX_PACKET_SIZE)
	{
		bytesTransferred = this->m_pSio->read(DVLpacket, numByte);
		if (numByte != bytesTransferred)
		{
			//printf(""4. Error: Expected to read " << numByte << " bytes but only read " << bytesTransferred << " (timeout)" << endl;
			return -1;
		}

		DVLpacket_tot[0] = 0x7F;
		DVLpacket_tot[1] = 0x7F;
		DVLpacket_tot[2] = dummyData[0];
		DVLpacket_tot[3] = dummyData[1];
		for(int j=4; j<this->packetLength; j++)
		{
			DVLpacket_tot[j] = DVLpacket[j-4];
		}

		uint16 receivedChsm = (DVLpacket[this->packetLength-6] << 8) + DVLpacket[this->packetLength-5];
		unsigned char cb1, cb2;
		uint16 computedChksm = computeChecksum(DVLpacket_tot, this->packetLength-2, cb1, cb2);
		//printf(""" << receivedChsm << "   VS  " << computedChksm << " 0x" << cb1 << "  0x" << cb2 << " 0x" <<  DVLpacket[this->packetLength-6] << " 0x" << DVLpacket[this->packetLength-5] << endl;
		if(receivedChsm == computedChksm)
		{
			//printf(""*************Ok Checksum!" << endl;
			this->convertData(DVLpacket_tot);
			//this->printReceivedPck(DVLpacket_tot, this->packetLength);
			return 0;
		}
		else
		{
			//printf(""+++++++++++++No Checksum!" << endl;
			//this->printReceivedPck(DVLpacket_tot, this->packetLength);
			return -3;
		}
	}
	else
		bytesTransferred = 0;

	return (bytesTransferred+4);

#endif


}


void DVL::convertData(unsigned char *packet)
{
	const int numDataTypes = packet[5];
	int cellNumb = -1;
	int *DataTypeAddr;
	if(numDataTypes > 0)
		DataTypeAddr = new int[numDataTypes];
	int j=6;
	int ensembleNumb = -1;
	int ensembleNumMSB = -1;
	int year = -1, month = -1, day = -1, hour = -1, minute = -1, second = -1, hundredths = -1;
	int soundVel = 0;
	float depth = -1.0, temperature = -1.0;
	int salinity = -1, pressure = -1, pressureVar = -1;
	//float BTrange[4], BTrangeMSB[4];
	float starboardVel = -1.0, fwdVel = -1.0, upwVel = -1.0, err = -1.0;
	float totalRange[4];
	float meanRange;
	uint16 tmp;
	float velTmp;
	int segno = -1;
	const unsigned char byteZero = 0x00;
	const double psi = M_PI/4.0;
	//const double psi = -PI/2.0;
	double cpsi, spsi;
	double vx, vy, vz;

	for(int i=0; i<numDataTypes; i++)
	{
		DataTypeAddr[i] = (packet[j+1] << 8) + packet[j];
		j += 2;
	}

	//if( (packet[6+2*numDataTypes] == 0x00) && (packet[6+2*numDataTypes+1] == 0x00) ) //Fixed leader ID
	if( (packet[DataTypeAddr[0]] == 0x00) && (packet[DataTypeAddr[0]+1] == 0x00) ) //Fixed leader ID
	{
		//.........................
		cellNumb = packet[DataTypeAddr[0]+10-1];
	}
	else
	{
		//...............
	}

	if((packet[DataTypeAddr[1]] == 0x80) && (packet[DataTypeAddr[1]+1] == 0x00)) //Variable leader ID
	{
		ensembleNumb = (packet[DataTypeAddr[1]+3] << 8) + packet[DataTypeAddr[1]+2];
		ensembleNumMSB = packet[DataTypeAddr[1]+11];
		year = packet[DataTypeAddr[1]+4];
		month = packet[DataTypeAddr[1]+5];
		day = packet[DataTypeAddr[1]+6];
		hour = packet[DataTypeAddr[1]+7];
		minute = packet[DataTypeAddr[1]+8];
		second = packet[DataTypeAddr[1]+9];
		hundredths = packet[DataTypeAddr[1]+10];
		soundVel = (packet[DataTypeAddr[1]+15] << 8) + packet[DataTypeAddr[1]+14];//m/s
		depth = 0.1*(float)((packet[DataTypeAddr[1]+17] << 8) + packet[DataTypeAddr[1]+16]);//m
		//heading
		//pitch
		//roll
		salinity = (packet[DataTypeAddr[1]+25] << 8) + packet[DataTypeAddr[1]+24];//ppt (part per thousand)
		temperature = 0.01*(float)((packet[DataTypeAddr[1]+27] << 8) + packet[DataTypeAddr[1]+26]);//degrees
		pressure =  (packet[DataTypeAddr[1]+51] << 24) + (packet[DataTypeAddr[1]+50] << 16) + (packet[DataTypeAddr[1]+49] << 8) + packet[DataTypeAddr[1]+48];//decaPascal
		pressureVar =  (packet[DataTypeAddr[1]+55] << 24) + (packet[DataTypeAddr[1]+54] << 16) + (packet[DataTypeAddr[1]+53] << 8) + packet[DataTypeAddr[1]+52];//decaPascal
	}
	else
	{
		//...........................
	}

	if((packet[DataTypeAddr[2]] == 0x00) && (packet[DataTypeAddr[2]+1] == 0x06)) //Bottom Track Data ID
	{
		//
		tmp = (packet[DataTypeAddr[2]+25] << 8) + packet[DataTypeAddr[2]+24];
		segno = (tmp & 32768) >> 15;
		//std::printf(""++++SEGNO: " << segno << std::endl;
		//std::printf(""++++TMP: " << std::hex << tmp << std::endl;
		surge_int=tmp;

		if(segno == 0)//Positivo
		{
			velTmp = (float)tmp;
		}
		else if(segno == 1)//Negativo
		{
			//two-complement
			tmp = ~tmp;
			tmp += 1;
			velTmp = -1.0*(float)(tmp);
		}
		else//Ibrido
		{
			//std::printf(""Di qui non dovrei passare...." << std::endl;
		}
		fwdVel = 0.001*velTmp;//m/s

		tmp = (packet[DataTypeAddr[2]+27] << 8) + packet[DataTypeAddr[2]+26];
		segno = (tmp & 32768) >> 15;

		sway_int=tmp;
		//std::printf(""++++SEGNO: " << segno << std::endl;
		//std::printf(""++++TMP: " << std::hex << tmp << std::endl;
		if(segno == 0)//Positivo
		{
			velTmp = (float)tmp;
		}
		else if(segno == 1)//Negativo
		{
			//two-complement
			tmp = ~tmp;
			tmp += 1;
			velTmp = -1.0*(float)(tmp);

		}
		else//Ibrido
		{
			//std::printf(""Di qui non dovrei passare...." << std::endl;
		}
		starboardVel = 0.001*velTmp;//m/s

		tmp = (packet[DataTypeAddr[2]+29] << 8) + packet[DataTypeAddr[2]+28];
		segno = (tmp & 32768) >> 15;

		heave_int=tmp;
		//std::printf(""++++SEGNO: " << segno << std::endl;
		//std::printf(""++++TMP: " << std::hex << tmp << std::endl;
		if(segno == 0)//Positivo
		{
			velTmp = (float)tmp;
		}
		else if(segno == 1)//Negativo
		{
			//two-complement
			tmp = ~tmp;
			tmp += 1;
			velTmp = -1.0*(float)(tmp);

		}
		else//Ibrido
		{
			//std::printf(""Di qui non dovrei passare...." << std::endl;
		}
		upwVel = 0.001*velTmp;//m/s

		tmp = (packet[DataTypeAddr[2]+31] << 8) + packet[DataTypeAddr[2]+30];
		segno = (tmp & 32768) >> 15;
		//std::printf(""++++SEGNO: " << segno << std::endl;
		//std::printf(""++++TMP: " << std::hex << tmp << std::endl;
		if(segno == 0)//Positivo
		{
			velTmp = (float)tmp;
		}
		else if(segno == 1)//Negativo
		{
			//two-complement
			tmp = ~tmp;
			tmp += 1;
			velTmp = -1.0*(float)(tmp);
		}
		else//Ibrido
		{
			//std::printf(""Di qui non dovrei passare...." << std::endl;
		}
		err = 0.001*velTmp;//m/s

		long pippolo = bytesToLong(byteZero, packet[DataTypeAddr[2]+77], packet[DataTypeAddr[2]+17], packet[DataTypeAddr[2]+16]);
		totalRange[0] = 0.01*(float)pippolo;

		pippolo = bytesToLong(byteZero, packet[DataTypeAddr[2]+78], packet[DataTypeAddr[2]+19], packet[DataTypeAddr[2]+18]);
		totalRange[1] = 0.01*(float)pippolo;

		pippolo = bytesToLong(byteZero, packet[DataTypeAddr[2]+79], packet[DataTypeAddr[2]+21], packet[DataTypeAddr[2]+20]);
		totalRange[2] = 0.01*(float)pippolo;

		pippolo = bytesToLong(byteZero, packet[DataTypeAddr[2]+80], packet[DataTypeAddr[2]+23], packet[DataTypeAddr[2]+22]);
		totalRange[3] = 0.01*(float)pippolo;

		meanRange = ((totalRange[0]*cos(M_PI/6.0)) + (totalRange[1]*cos(M_PI/6.0)) + (totalRange[2]*cos(M_PI/6.0)) + (totalRange[3]*cos(M_PI/6.0)))/4.0;
	}
	else
	{
		//...........................
	}


	cpsi = cos(psi);
	spsi = sin(psi);

	//Vx = [cpsi -spsi 0; spsi cspi 0; 0 0 1];

	vx = cpsi*fwdVel - spsi*starboardVel;
	vy = -1.0*(spsi*fwdVel + cpsi*starboardVel);
	vz = upwVel;

	/*
	R2DVLData->surgeVelocity = vx;
	R2DVLData->swayVelocity = vy;
	R2DVLData->heaveVelocity = vz;
	R2DVLData->errorVelocity = err;

	R2DVLData->range1 = totalRange[0];
	R2DVLData->range2 = totalRange[1];
	R2DVLData->range3 = totalRange[2];
	R2DVLData->range4 = totalRange[3];
	 */

	//printf("DVL: surge: %x   sway: %x   heave: %x\n",surge_int,sway_int,heave_int);
	//printf("fwd: %f   stbd: %f   up: %lf\n",fwdVel,starboardVel,upwVel);
	if (surge_int==0x8000 || sway_int==0x8000 || heave_int==0x8000) validVelocity=0;
	else validVelocity=1;

	surgeVelocity = vx;
	swayVelocity = vy;
	heaveVelocity = vz;
	errorVelocity = err;

	su=0.5*su+0.5*vx;
	sv=0.5*sv+0.5*vy;
	//surgeVelocity = su;
	//swayVelocity = sv;

	range1 = totalRange[0];
	range2 = totalRange[1];
	range3 = totalRange[2];
	range4 = totalRange[3];
	altitude=(range1+range2+range3+range4)*cos(RANGE_ANGLE)/4.0;

	//fprintf(stderr, "%lf  %lf  %lf  %lf  |  %lf  %lf  %lf  %lf\n",totalRange[0],totalRange[1],totalRange[2],totalRange[3],surgeVelocity,swayVelocity,heaveVelocity,errorVelocity);

	//printf(""Temp: " << temperature << endl;

	delete [] DataTypeAddr;
	DataTypeAddr = NULL;

	return;
}


uint16 DVL::computeChecksum(unsigned char *dvl_packet, int pckSize, unsigned char &b1, unsigned char &b2)
{
	int chksm = 0;
	int i=0;
	uint16 finalChksm = 0;
	for(i=0; i<pckSize; i++)
	{
		chksm += dvl_packet[i];
	}
	if(chksm>65535)
		chksm = (chksm % 65535);

	b1 = chksm & 0xFF;
	b2 = (chksm >> 8);

	finalChksm = (b1 << 8) + b2;

	return finalChksm;
}


void* start_dvl(void *arg)
{
	DVL* pThread=(DVL*)((ThreadInfo*)arg)->pThread;
	pThread->execute();
	return NULL;
}
