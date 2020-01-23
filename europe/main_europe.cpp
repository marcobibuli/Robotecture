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
	loopSleep.tv_sec = CORE_SAMPLE_TIME_SEC;
	loopSleep.tv_nsec = CORE_SAMPLE_TIME_NSEC;

	char localIp[256] = "10.17.20.109";
	char remoteIp[256] = "10.17.20.109";
	uint16 localPort = (uint16)10000;
	uint16 remotePort = (uint16)10001;

	CommLink *cl=new CommLink("europe", CommMode::HARD_ACK);

	cl->open(localIp, localPort, remoteIp, remotePort);
	cl->create();

	long num = 123;
	char msg[1024];
	int ret=-1;
	int flag = 1;

	while (running)
	{
		//if (flag==1)
		{
			sprintf(msg, "%ld", num++);
			cl->send_message(msg, strlen(msg));
			sprintf(msg, "");
			flag = 0;
		}

		
		while ((ret = cl->recv_message(msg)) > 0)
		{
			msg[ret] = NULL;
			printf("%d | %s\n", ret, msg);
			flag = 1;
			sprintf(msg, "");
		}
		
		nanosleep(&loopSleep, NULL);
	}


	cl->terminate();
}