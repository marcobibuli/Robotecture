
#define __MAIN__

#include <stdio.h>
#include <string.h>

#include "europe_status.h"

#include "../common/generic/define.h"
#include "../common/network/CommLink.h"
#include "../common/network/NetworkManager.h"



bool running = true;

europe_status *status;

NetworkManager networkManager;




void ctrl_c_handler(int)
{
	running = false;
}


int main()
{
	timespec loopSleep;
	loopSleep.tv_sec = CORE_SAMPLE_TIME_SEC;
	loopSleep.tv_nsec = CORE_SAMPLE_TIME_NSEC;

	strcpy(ROBOT_NAME, "europe");


	status = new europe_status("europeStatus");
	status->create();


	if (networkManager.init() == -1)
	{
		printf("NetworkManager init error\n");
		return -1;
	}


	CommLink* cl_in = new CommLink("HeartBeat_commLink_in", UDP_PURE);
	CommLink* cl_out = new CommLink("HeartBeat_commLink_out", UDP_PURE);

	cl_in->open(networkManager.ROBOT_IP, networkManager.HEARTHBEAT_ROBOT_PORT_IN,
		networkManager.HMI_IP, networkManager.HEARTHBEAT_HMI_PORT_OUT);

	cl_out->open(networkManager.ROBOT_IP, networkManager.HEARTHBEAT_ROBOT_PORT_OUT,
		networkManager.HMI_IP, networkManager.HEARTHBEAT_HMI_PORT_IN);

	cl_in->create();
	cl_out->create();
	
	
	Time_status time_status;
	int br;
	Message msg;

	int64 zzz = 123;

	char m[256];
	sprintf(m,"123456");

	printf("%d %d %d %d %d %d\n", m[0], m[1], m[2], m[3], m[4], m[5]);

	/*
	int L = strlen(m);

	printf("L: %d    LL: %d\n", L, (L / 4) * 4);

	uint32* ptr = (uint32*)m;
	for (int i = 0; i < (L / sizeof(uint32)); i++,ptr++)
	{
		printf("i: %d\n",i);
		*ptr = htonl(*ptr);
	}

	

	printf("%d %d %d %d %d %d\n",m[0],m[1],m[2],m[3],m[4],m[5]);
	*/
	
	cl_out->send_message((char*)&zzz,sizeof(zzz),false);

	while (running)
	{
		/*
		time_status = status->time_status.get();

		if (cl_out->getLinkLevel() == NORMAL) cl_out->send_message((char*) & (time_status.timeStamp), sizeof(time_status.timeStamp));

		do {
			br = cl_in->recv_message((char*)&msg);
		} while (br > 0);
		*/
		
		nanosleep(&loopSleep, NULL);
	}

	status->terminate();
	
}