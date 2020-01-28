#include <stdio.h>
#include "../common/network/CommLink.h"


bool running = true;

void ctrl_c_handler(int)
{
	running = false;
}


int main()
{
	timespec loopSleep;
	//loopSleep.tv_sec = CORE_SAMPLE_TIME_SEC;
	//loopSleep.tv_nsec = CORE_SAMPLE_TIME_NSEC;
	loopSleep.tv_sec = 1;
	loopSleep.tv_nsec = 0;


	char localIp[256] = "10.17.20.109";
	char remoteIp[256] = "10.17.20.109";
	uint16 localPort = (uint16)10001;
	uint16 remotePort = (uint16)10000;

	CommLink cl("europe", CommMode::OVERRIDE);

	cl.open(localIp, localPort, remoteIp, remotePort);
	cl.create();

	long num = -1;
	char msg[1024]="";
	int ret;
	int flag = 0;
	
	while(running)
	{
		sprintf(msg, "");
		while ((ret = cl.recv_message(msg)) > 0)
		{
			msg[ret] = NULL;
			printf("%d | %s\n", ret, msg);
			flag = 1;
			sprintf(msg, "");
		}

		
		//if (flag == 1)
		/*
		{
			sprintf(msg, "%ld", num--);
			cl.send_message(msg, strlen(msg));
			flag = 0;
		}
		*/
		
		nanosleep(&loopSleep, NULL);
	}

	cl.terminate();
}