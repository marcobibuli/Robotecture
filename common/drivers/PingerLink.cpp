/*
 * PingerLink.cpp
 *
 *  Created on: May 9, 2018
 *      Author: mx
 */

#include "PingerLink.h"

#define PINGER_LINK_THREAD_PRIORITY 80


PingerLink::PingerLink(const char *name,NetworkManager &nm,Status *s):Device(name,SCHED_FIFO,PINGER_LINK_THREAD_PRIORITY,start_PingerLink,nm,s)
{
	strcpy(configFileName,CONFIGURATION_PINGER_FILE);

	range=-1.0;
	sequence=0;
	pinger_status=RECEIVE;

	sem_init(&statusMutex,0,1);

	inLink=NULL;
	outLink=NULL;
	serialPort=0;

	srand(time(NULL));

	pos_x=0.0;
	pos_y=0.0;
	pos_z=0.0;


}


PingerLink::~PingerLink()
{
	if (inLink!=NULL)
	{
		inLink->close();
		delete inLink;
	}

	if (outLink!=NULL)
	{
		outLink->close();
		delete outLink;
	}
}


int PingerLink::init_sim()
{
	printf("PingerLink::init_sim()\n");

	outLink=new CommLink( "Pinger_out" , OVERRIDE );
	outLink->open( networkManager->ROBOT_IP , networkManager->PINGER_SIM_PORT_OUT ,
			       networkManager->USBL_IP   , networkManager->USBL_SIM_PORT_IN );
	outLink->create();

	inLink=new CommLink( "Pinger_in" , OVERRIDE );
	inLink->open( networkManager->ROBOT_IP , networkManager->PINGER_SIM_PORT_IN ,
			      networkManager->USBL_IP   , networkManager->USBL_SIM_PORT_OUT );
	inLink->create();

	device_status=DEVICE_OFF;

	return 0;
}


int PingerLink::init_act()
{
	//printf("Pinger init complete.\n\n\n");
	device_status=DEVICE_OFF;

	return 0;
}


int PingerLink::init_pinger()
{
	if (device_mode==DEVICE_ACT)
	{
		usleep(500000);


		unsigned int baudRate=B19200;
		unsigned int dataBits=CS8;
		unsigned int stopBits=0;
		unsigned int parityEnable=0;
		unsigned int parityType=0;
		unsigned int receivingEnable=CREAD;
		unsigned int noBlockingFlag=0;


		struct termios oldTio,newTio;

		if((serialPort=open(pinger_serial_name,O_RDWR|noBlockingFlag /* | O_NOCTTY | O_NDELAY */))<0)
		{
			printf("Serial Port error - open %s\n",strerror(errno));
			return(-1);
		}


		if(tcflush(serialPort,TCIOFLUSH)<0)
		{
			printf("Serial Port error - flush %s\n",strerror(errno));
			return(-1);
		}


		tcgetattr(serialPort,&oldTio);

		memset(&newTio,0,sizeof(newTio));


		// set iflag values for raw management
		newTio.c_iflag=IGNBRK|IGNPAR;
		//newTio.c_iflag=0;
		//newTio.c_oflag=0;
		//newTio.c_lflag=0;

		// set cflag values
		newTio.c_cflag=dataBits|stopBits|parityEnable|
				parityType|receivingEnable|CLOCAL;
		newTio.c_cc[VMIN] =1; // blocking read until 1 char received
		newTio.c_cc[VTIME]=0; // inter-character timer not used

		// set output speed
		if(cfsetospeed(&newTio,baudRate)<0)
		{
			printf("Serial Port error - cfsetospeed %s\n",strerror(errno));
			return(-1);
		}

		// set input speed
		if(cfsetispeed(&newTio,baudRate)<0)
		{
			printf("Serial Port error - cfsetospeed %s\n",strerror(errno));
			return(-1);
		}

		tcsetattr(serialPort,TCSANOW,&newTio);




		char buf[256];
		int bufLength=0;

		char out[BUF_SIZE];
		char fullString[BUF_SIZE];
		int bytesread;

		usleep(10000000);



		out[0]='\0';
		fullString[0]='\0';
		int count=0;

		//while(strstr(fullString,"OK")==NULL)
		printf("Ready to init serial %d\n",serialPort);
		{

			sprintf(buf,"+");
			bufLength=strlen(buf);
			write(serialPort,buf,bufLength);

			usleep(50000);

			sprintf(buf,"+");
			bufLength=strlen(buf);
			write(serialPort,buf,bufLength);

			usleep(50000);

			sprintf(buf,"+");
			bufLength=strlen(buf);
			write(serialPort,buf,bufLength);

			printf("PINGER - %s\n",buf);
			usleep(3000000);

			count=0;

			while(strstr(fullString,"OK")==NULL)
			{
				bytesread=read(serialPort,out,BUF_SIZE);
				if (bytesread>=0)
				{
					out[bytesread]='\0';
					strcat(fullString,out);
				}

				printf("Pinger string: %s\n",fullString);
				usleep(100000);
				count++;
			}
		}

			printf("Init +++: %s\n",fullString);


			usleep(500000);


		printf("Ready to init serial %d\n",serialPort);

		sprintf(buf,"AT@ZX1\r");
		bufLength=strlen(buf);
		write(serialPort,buf,bufLength);

		while(strstr(fullString,"OK")==NULL)
		{
			bytesread=read(serialPort,out,BUF_SIZE);
			if (bytesread>=0)
			{
				out[bytesread]='\0';
				strcat(fullString,out);
			}
			usleep(100000);
		}


		printf("Init AT@ZX1: %s\n",fullString);
	}

	printf("Pinger init complete.\n");
	return 0;
}


char* PingerLink::extractPosition(char *dataBuf)
{
	sequence=dataBuf[0];

	char xs[10],ys[10],zs[10];
	int i=1,j=0;

	do{
		xs[j++]=dataBuf[i++];
	}while(dataBuf[i]!='+' && dataBuf[i]!='-' && dataBuf[i]!=',');
	long xl=strtol(xs+1,NULL,10);
	if (xs[0]=='-') xl=-xl;
	pos_x=((double)(xl))/100.0;

	j=0;
	do{
		ys[j++]=dataBuf[i++];
	}while(dataBuf[i]!='+' && dataBuf[i]!='-' && dataBuf[i]!=',');
	long yl=strtol(ys+1,NULL,10);
	if (ys[0]=='-') yl=-yl;
	pos_y=((double)(yl))/100.0;

	j=0;
	do{
		zs[j++]=dataBuf[i++];
	}while(dataBuf[i]!='+' && dataBuf[i]!='-' && dataBuf[i]!=',');
	long zl=strtol(zs+1,NULL,10);
	if (zs[0]=='-') zl=-zl;
	pos_z=((double)(zl))/100.0;
	i++;

	//printf("xl: %lf    yl: %lf    zl: %lf\n",pos_x,pos_y,pos_z);

	return (dataBuf+i);
}


void PingerLink::execute_sim()
{
	timespec tSleep;
	tSleep.tv_sec=PINGER_SLEEP_SEC;
	tSleep.tv_nsec=PINGER_SLEEP_NSEC;

	Pinger_sim_packet dataBuf;

	while(running)
	{
		int ret;
		do{
			ret=inLink->recv_message((char*)&dataBuf);
			if (ret>0)
			{
				char *msg=extractPosition(dataBuf.data);
				strcpy(msgIn.data,msg);
				pingerStatus(RECEIVED);
			}

		}while(ret>0);

		nanosleep(&tSleep,NULL);
	}


	inLink->terminate();
	while(!(inLink->isTerminated())) {nanosleep(&tSleep,NULL); }
	delete inLink;
	inLink=NULL;

	outLink->terminate();
	while(!(outLink->isTerminated())) {	nanosleep(&tSleep,NULL); }
	delete outLink;
	outLink=NULL;

	terminated=true;
}



void PingerLink::execute_act()
{
	timespec tSleep;
	tSleep.tv_sec=PINGER_SLEEP_SEC;
	tSleep.tv_nsec=PINGER_SLEEP_NSEC;

	int bytesread=0;
	char out[BUF_SIZE];
	char fullString[BUF_SIZE];

	char ts_buf[100];

	while(running)
	{
		bytesread=read(serialPort,out,BUF_SIZE);
		if (bytesread>0)
		{
			out[bytesread]='\0';
			strcat(fullString,out);
			//printf("fullString: %s\n",fullString);

			char dataBuf[100];


			char *sendend=strstr(fullString,"SENDEND"); //grab send timeStamp
			if (sendend!=NULL && pingerStatus()==SENDING)
			{
				int j=0,k=0;
				for(int i=0;i<3;)
					if (sendend[j++]==',') i++;

				while(sendend[j]!=',')
					ts_buf[k++]=sendend[j++];
				ts_buf[k]='\0';
				msgOut.timeStamp=strtoul(ts_buf,NULL,10);

				pingerStatus(SENT);
			}



			char *recvstart=strstr(fullString,"RECVSTART"); //grab reception timeStamp
			if (recvstart!=NULL && pingerStatus()==RECEIVE)
			{
				pingerStatus(RECEIVING);
			}



			char *recvend=strstr(fullString,"RECVEND"); //grab reception timeStamp
			if (recvend!=NULL && pingerStatus()==RECEIVING)
			{
				int j=0,k=0;
				for(int i=0;i<1;)
					if (recvend[j++]==',') i++;

				while(recvend[j]!=',')
					ts_buf[k++]=recvend[j++];
				ts_buf[k]='\0';
				msgIn.timeStamp=strtoul(ts_buf,NULL,10);
			}




			char *recvim=strstr(out,"RECVIM");
			if (recvim!=NULL && pingerStatus()==RECEIVING)
			{
				int len=0;
				char len_buf[100];
				int j=0,k=0;
				for(int i=0;i<1;)
					if (recvim[j++]==',') i++;

				while(recvim[j]!=',')
					len_buf[k++]=recvim[j++];
				len_buf[k]='\0';
				len=strtoul(len_buf,NULL,10);


				j=0,k=0;
				for(int i=0;i<9;)
					if (recvim[j++]==',') i++;

				while(k<len)
					dataBuf[k++]=recvim[j++];
				dataBuf[k]='\0';

				fullString[0]='\0';

				char *msg=extractPosition(dataBuf);

				strcpy(msgIn.data,msg);



				pingerStatus(RECEIVED);

				//printf("Message: %s   ts: %u\n",m->data,m->timeStamp);
			}

			char *cancelledim=strstr(out,"CANCELLEDIM");
			if (cancelledim!=NULL && pingerStatus()==SENDING)
			{
				fullString[0]='\0';
				pingerStatus(RECEIVE);
			}


			char *error=strstr(out,"ERROR");
			if (error!=NULL && pingerStatus()==SENDING)
			{
				fullString[0]='\0';
				pingerStatus(RECEIVE);
			}


			char *recvfailed=strstr(out,"RECVFAILED");
			if (recvfailed!=NULL)
			{
				fullString[0]='\0';
			}

		}

		nanosleep(&tSleep,NULL);
	}
}

void PingerLink::getPosition(double &x,double &y,double &z)
{
	x=pos_x;
	y=pos_y;
	z=pos_z;
}

char* PingerLink::receivePingerMessage()
{
	return msgIn.data;
}


void PingerLink::sendPingerMessage(char* m)
{
	if (pingerStatus()==SEND)
	{
		pingerStatus(SENDING);

		char buf[BUF_SIZE];
		unsigned int dataLen=strlen(m);
		strcpy(msgOut.data,m);

		sprintf(buf,"AT*SENDIM,%d,%d,noack,%c%s\r",dataLen+1,USBL_ADDR,sequence,m);

		if (device_mode==DEVICE_SIM) sendPingerMessage_sim(m,dataLen);
		else if (device_mode==DEVICE_ACT) sendPingerMessage_act(buf);
	}
}

void PingerLink::sendPingerMessage_sim(char* buf,int bufLength)
{
	if (rand() % 100 != 0) outLink->send_message(buf,bufLength);
	pingerStatus(SENT);
}

void PingerLink::sendPingerMessage_act(char* buf)
{
	write(serialPort,buf,strlen(buf));
}



void* start_PingerLink(void *arg)
{
	PingerLink* pThread=(PingerLink*)((ThreadInfo*)arg)->pThread;
	pThread->execute();
	return NULL;
}



