/*
 * FOG.cpp
 *
 *  Created on: Jun 6, 2018
 *      Author: mx
 */


#include "FOG.h"

FOG::FOG(const char *name,NetworkManager &nm, DataAccess<Time_status>& Time_access, DataAccess<FOG_status>& Fog_access):Device(name,SCHED_FIFO,FOG_THREAD_PRIORITY,start_fog,nm)
{
	mpADatagram=NULL;
	networkManager=&nm;
	time_access = &Time_access;
	fog_access = &Fog_access;

	packetLength=67;
	timeUsecResolution = (15.26);
	GPSResolution = (180.0/pow(2.0, 31.0));
	AngleResolution = (180.0/pow(2.0, 23.0));

	strcpy(configFileName,CONFIGURATION_FOG_FILE);

	NavData=new LongBinNavHRDataStruct;
	RobotNavData=new Robot_LongBinNavHR;

	/*
	tlmSim = new CommLink("FOG_tlmSim", UDP_PURE);
	tlmSim->open(networkManager->ROBOT_IP, networkManager->FOG_ROBOT_PORT_IN,
		         networkManager->SIM_IP, networkManager->FOG_SIM_PORT_OUT);
	tlmSim->create();
	*/

	device_status = DEVICE_RUNNING;
}

FOG::~FOG()
{
	delete NavData;
	delete RobotNavData;
}


int FOG::init_sim()
{
	printf("FOG::init_sim()\n");
	running_sim = true;

	device_status = DEVICE_RUNNING;

	mpADatagram = new CommLink("FOGSim_tlm", UDP_PURE);
	mpADatagram->open( networkManager->ROBOT_IP, networkManager->FOG_ROBOT_SIM_PORT_IN,
		               networkManager->SIM_IP, networkManager->FOG_SIM_PORT_OUT );
	mpADatagram->create();

	return 0;
}


int FOG::init_act()
{
	printf("FOG::init_act()\n");
	running_act=true;

	/*
	mpADatagram = new Datagram();
	mpADatagram->open(13086);
	mpADatagram->enable_blocking();
	*/

	mpADatagram=new CommLink("FOG_commLink" , UDP_PURE);
	mpADatagram->enable_blocking();

	if (mpADatagram->open("192.168.1.125",FOG_QUADRANS_PORT_NAV, NULL, -1) != 0)
	//if (mpADatagram->open(FOG_QUADRANS_PORT_NAV, 0, -1) != 0)
	{
		printf("FOG socket error\n");
		return -1;
	}

	device_status= DEVICE_RUNNING;

	return 0;
}


void FOG::execute_sim()
{
	timespec tSleep;
	tSleep.tv_sec=FOG_SLEEP_SEC;
	tSleep.tv_nsec=FOG_SLEEP_NSEC;

	while(running_sim)
	{
		read_sim_tlm();

		nanosleep(&tSleep,NULL);
	}
}


void FOG::execute_act()
{
	char *message = new char[packetLength];


	int received = 0;

	timespec tSleep;
	tSleep.tv_sec=FOG_SLEEP_SEC;
	tSleep.tv_nsec=FOG_SLEEP_NSEC;

	while(running_act)
	{
		received = receive(message);

		if (received < 0)
		{
			//printf("FOG - ERROR reading message.\n");

		}

		send_telemetry();

		nanosleep(&tSleep,NULL);
	}
}


int FOG::receive(char *recMessage)
{
	device_status=DEVICE_RUNNING;
	int received = 0;

	received = mpADatagram->recv(recMessage, packetLength, 0 );

	if (received == -1)
	{
		//printf("FOG - Receive error.\n");
		device_status=DEVICE_FAULT;
		return received;
	}



	NavData = (LongBinNavHRDataStruct*)(recMessage);
    //printf("size of: %d\n",sizeof(LongBinNavHRDataStruct));
	//printf("received: %d    crc: %x %x     %x %x\n",received,recMessage[65],recMessage[66],((char*)(NavData->CRC))[0],((char*)(NavData->CRC))[1]);

	if(this->check_crc((unsigned char*)recMessage) == 0)
	{
		convertData();
		update_device_status(received);
	}

	else
	{
		received = -3;
	}


	return received;
}


void FOG::convertData()
{
	Int24 RollAngle, PitchAngle, HeadingAngle, HeadingRate, RollRate, PitchRate;
	int32_t ChRollAngle, ChPitchAngle, ChHeadingAngle, ChHeadingRate, ChRollRate, ChPitchRate;
	//uint32_t time_s, time_m, time_h, time_d, time_residual, time_s_tot, time_ns_32;
	//uint32_t tmp1;
	//uint16_t time_ns;
	int32_t tempInt32;
	int16_t tempInt16;
	//uint64_t time_ns_prev = 0;

	/*//TIMESTAMP>
	time_s_tot = ntohl(NavData->secs);
	time_ns = ntohs(NavData->usecs);
	//time_ns = (uint16_t)((float)time_ns*CONST_TIME_USEC_RES)*1000;
	time_ns_32 = (uint32_t)(time_ns*1000*this->timeUsecResolution);
	time_d = time_s_tot/(3600*24); //DAYS
	time_residual = time_s_tot % (3600*24); //RESIDUAL TIME AFTER DAY COMPUTATION
	time_h = time_residual/3600; //HOURS
	tmp1 = time_residual;
	time_residual = tmp1 % 3600; //RESIDUAL TIME AFTER HOUR COMPUTATION

	time_m = time_residual/60; //MINUTES
	tmp1 = time_residual;
	time_residual = tmp1 % 60; //RESIDUAL TIME AFTER MINUTE COMPUTATION
	time_s = time_residual;

	CharlieNavData->timestamp = (uint64_t)(time_s_tot)*1000000000L + (uint64_t)(time_ns_32);
	//TIMESTAMP*/

	/*//LATITUDE & LONGITUDE
	tempInt32 = ntohl(NavData->latitude);
	CharlieNavData->latitude = (float)((tempInt32)*this->GPSResolution);
	tempInt32 = ntohl(NavData->longitude);
	CharlieNavData->longitude = (float)((tempInt32)*this->GPSResolution);
	//LATITUDE & LONGITUDE*/

	//ALTITUDE
	tempInt32 = ntohl(NavData->altitude);
	RobotNavData->altitude = (float)(tempInt32/1000.0); //in m
	//ALTITUDE

	//HEAVE
	tempInt16 = ntohs(NavData->heave);
	RobotNavData->heave = (float)(tempInt16/1000.0); //in m
	//HEAVE

	//LINEAR VELOCITIES
	tempInt16 = ntohs(NavData->Nvel);
	RobotNavData->Nvel = (float)(tempInt16/1000.0); //in m/s
	tempInt16 = ntohs(NavData->Evel);
	RobotNavData->Evel = (float)(tempInt16/1000.0); //in m/s
	tempInt16 = ntohs(NavData->Dvel);
	RobotNavData->Dvel = (float)(tempInt16/1000.0); //in m/s
	//LINEAR VELOCITIES

	//ANGLES & ANGULAR VELOCITIES
	RollAngle = (NavData->roll);
	PitchAngle = (NavData->pitch);
	HeadingAngle = (NavData->heading);
	HeadingRate = (NavData->headingRate);
	RollRate = (NavData->rollRate);
	PitchRate = (NavData->pitchRate);

	RollAngle.swap();
	PitchAngle.swap();
	HeadingAngle.swap();
	HeadingRate.swap();
	RollRate.swap();
	PitchRate.swap();

	ChRollAngle = ((int32_t)(RollAngle));
	ChPitchAngle = ((int32_t)(PitchAngle));
	ChHeadingAngle = ((int32_t)(HeadingAngle));
	ChHeadingRate = ((int32_t)(HeadingRate));
	ChRollRate = ((int32_t)(RollRate));
	ChPitchRate = ((int32_t)(PitchRate));


	RobotNavData->roll = ChRollAngle*AngleResolution; //deg
	RobotNavData->pitch = ChPitchAngle*AngleResolution;//deg
	RobotNavData->pitch *= -1.0;
	RobotNavData->heading = ChHeadingAngle*AngleResolution;//deg
	if(RobotNavData->heading < 0)
		RobotNavData->heading += 360.0;
	RobotNavData->heading = mod180(RobotNavData->heading);
	RobotNavData->headingRate = ChHeadingRate*AngleResolution;//deg/s
	RobotNavData->rollRate = ChRollRate*AngleResolution;//deg/s
	RobotNavData->pitchRate = ChPitchRate*AngleResolution;//deg/s
	//ANGLES & ANGULAR VELOCITIES
}


int FOG::check_crc(unsigned char *message)
{
	unsigned char *checkCRCbuf = new unsigned char[this->packetLength-3];
	for (int i=0; i<this->packetLength-3; i++)
		checkCRCbuf[i] = message[i+1];

	unsigned short crc_ret = blkcrc(checkCRCbuf, this->packetLength-3);
	unsigned short crc_packet = ntohs(NavData->CRC);

	if (crc_ret != crc_packet)
	{
		RobotNavData->isValid = false;
		//cout << "Non valid data" << endl;
		delete checkCRCbuf;
		checkCRCbuf = NULL;
		return (-1);
	}
	else
	{
		RobotNavData->isValid = true;
		//cout << "Valid data" << endl;
	}
	delete checkCRCbuf;
	checkCRCbuf = NULL;
	return 0;
}


unsigned short FOG::blkcrc(unsigned char* bufptr, unsigned len)
{
	//dprintf("FOG_LongBinNavHRProtocol::blkcrc(unsigned char*, unsigned)");
	unsigned char i;
	unsigned short data;
	unsigned short crc = 0xffff;

	if (len == 0)
		return ~crc;
	do
	{
		for (i = 0, data = (unsigned short)(0xff & *bufptr++); i
			< 8; i++, data >>= 1)
		{
			if ((crc & 0x0001) ^ (data & 0x0001))
			{
				crc = (crc >> 1) ^ 0x8408;
			}
			else
			{
				crc >>= 1;
			}
		}
	} while (--len);

	crc = ~crc;
	data = crc;
	crc = (crc << 8) | ((data >> 8) & 0xff);

	return crc;
}


void FOG::read_sim_tlm()
{
	int ret;
	FOG_tlm_packet tp;

	do {
		ret = mpADatagram->recv_message((char*)&tp);
		
		if (ret > 0)
		{
			RobotNavData->roll = ((double)tp.roll) / FOG_factor;
			RobotNavData->pitch = ((double)tp.pitch) / FOG_factor;
			RobotNavData->heading = ((double)tp.heading) / FOG_factor;
			RobotNavData->rollRate = ((double)tp.rollRate) / FOG_factor;
			RobotNavData->pitchRate = ((double)tp.pitchRate) / FOG_factor;
			RobotNavData->headingRate = ((double)tp.headingRate) / FOG_factor;

			updateStatus();
		}
		
		update_device_status(ret);

	} while (ret > 0);
}


void FOG::updateStatus()
{
	FOG_status fog_status;
	Time_status time_status;

	fog_status = fog_access->get();
	time_status = time_access->get();

	fog_status.roll.value=RobotNavData->roll * M_PI / 180.0 ;
	fog_status.pitch.value =RobotNavData->pitch * M_PI / 180.0 ;
	fog_status.heading.value =modpi(RobotNavData->heading*M_PI/180.0);
	fog_status.rollRate.value =RobotNavData->rollRate * M_PI / 180.0 ;
	fog_status.pitchRate.value =RobotNavData->pitchRate * M_PI / 180.0 ;
	fog_status.headingRate.value =RobotNavData->headingRate * M_PI / 180.0 ;

	fog_status.roll.valid = RobotNavData->isValid;
	fog_status.pitch.valid = RobotNavData->isValid;
	fog_status.heading.valid = RobotNavData->isValid;
	fog_status.rollRate.valid = RobotNavData->isValid;
	fog_status.pitchRate.valid = RobotNavData->isValid;
	fog_status.headingRate.valid = RobotNavData->isValid;

	fog_status.roll.timeStamp = time_status.timeStamp;
	fog_status.pitch.timeStamp = time_status.timeStamp;
	fog_status.heading.timeStamp = time_status.timeStamp;
	fog_status.rollRate.timeStamp = time_status.timeStamp;
	fog_status.pitchRate.timeStamp = time_status.timeStamp;
	fog_status.headingRate.timeStamp = time_status.timeStamp;

	fog_status.device_status = DEVICE_RUNNING;

	fog_access->set(fog_status);
}


void FOG::update_device_status(int r)
{
	FOG_status fog_status;
	fog_status = fog_access->get();

	if (r > 0)
	{
		missed_receive_count = 0;
		device_status = DEVICE_RUNNING;
	}

	else if (r <= 0)
	{
		//printf("FOG - ERROR reading message.\n");
		missed_receive_count++;
		if (missed_receive_count > FOG_RECEIVE_WAIT_LOOPS_FOR_WARNING && missed_receive_count <= FOG_RECEIVE_WAIT_LOOPS_FOR_FAULT)
		{
			device_status = DEVICE_WARNING;
		}

		else if (missed_receive_count > FOG_RECEIVE_WAIT_LOOPS_FOR_FAULT)
		{
			device_status = DEVICE_FAULT;
			missed_receive_count = FOG_RECEIVE_WAIT_LOOPS_FOR_FAULT;
		}
	}

	fog_status.device_status = device_status;

	fog_access->set(fog_status);
}



void FOG::send_telemetry()
{
	FOG_tlm_packet msg;

	msg.roll=(int64)((RobotNavData->roll)*FOG_factor);
	msg.pitch=(int64)((RobotNavData->pitch)*FOG_factor);
	msg.heading=(int64)((mod180(RobotNavData->heading))*FOG_factor);
	msg.rollRate=(int64)((RobotNavData->rollRate)*FOG_factor);
	msg.pitchRate=(int64)((RobotNavData->pitchRate)*FOG_factor);
	msg.headingRate=(int64)((mod180(RobotNavData->headingRate))*FOG_factor);
	msg.device_status=device_status;

	tlm->send_message((char*)&msg,sizeof(msg));
}


void* start_fog(void *arg)
{
	FOG* pThread=(FOG*)((ThreadInfo*)arg)->pThread;
	pThread->execute();
	return NULL;
}
