/*
 * UsblLink.cpp
 *
 *  Created on: May 9, 2018
 *      Author: mx
 */


#include "UsblLink.h"

#define USBL_LINK_THREAD_PRIORITY 80


UsblLink::UsblLink(const char *name,NetworkManager &nm):RobotThread(name,SCHED_FIFO,USBL_LINK_THREAD_PRIORITY,start_UsblLink)
{
	srand(time(NULL));

	range=-1.0;
	sequence='0';
	pos_x=0.0;
	pos_y=0.0;
	pos_z=0.0;

	networkManager=&nm;

	sem_init(&statusMutex,0,1);

	FILE *f;
	char value[100],name_dir_file[256];

	//sprintf(name_dir_file,"%s%s",configDir,configFileName);
	sprintf(CONFIGURATION_FILES_DIRECTORY, "../../../%s/config/", ROBOT_NAME);
	sprintf(name_dir_file, "%s%s", CONFIGURATION_FILES_DIRECTORY, configFileName);
	

	if((f=fopen(name_dir_file,"r"))!=NULL)
	{
		while(!feof(f))
		{
			fscanf(f,"%s\n",value);
		}

		fclose(f);

		if (strcmp(value,"SIM")==0) init_sim();
		else if (strcmp(value,"ACT")==0) init_act();
	}

	else
	{
		printf("Device::init - File error: %s\n",name_dir_file);
		exit(-1);
	}
}


int UsblLink::init_sim()
{
	device_mode=DEVICE_SIM;

	outLink=new CommLink( "Pinger_out" , OVERRIDE );
	outLink->open( networkManager->USBL_IP  , networkManager->USBL_SIM_PORT_OUT ,
			       networkManager->ROBOT_IP , networkManager->PINGER_SIM_PORT_IN );
	outLink->create();


	inLink=new CommLink( "Pinger_in" , OVERRIDE );
	inLink->open( networkManager->USBL_IP  , networkManager->USBL_SIM_PORT_IN ,
			      networkManager->ROBOT_IP , networkManager->PINGER_SIM_PORT_OUT );
	inLink->create();


	posLink=new CommLink( "USBLpos_Link" , UDP_PURE );
	posLink->open( networkManager->USBL_IP , networkManager->USBL_POS_PORT_IN ,
			       networkManager->SIM_IP  , networkManager->USBL_POS_SIM_PORT_OUT );
	posLink->create();

	printf("USBL init complete.\n\n\n");

	return 0;
}


int UsblLink::init_act()
{
	device_mode=DEVICE_ACT;

	cl=new Client();
	cl->open( USBL_IP_ADDR , USBL_PORT );


	printf("Start init...\n");

	char buf[BUF_SIZE];
	char fwversion[BUF_SIZE];
	int bytesread=0;
	while(bytesread==0)
	{
		bytesread=read_from_usbl(getFWVersion,buf);

	}

	bytesread=read_from_usbl(getFWVersion,buf);
	bytesread=read_from_usbl(getFWVersion,buf);
	buf[bytesread]='\0';
	strcpy(fwversion,buf);

	printf("version...\n");
	bytesread=read_from_usbl(getFWVersionMinor,buf);
	strcat(fwversion,buf);

	bytesread=read_from_usbl(getFWVersionBrand,buf);
	strcat(fwversion,buf);

	printf("FW version: %s\n\n",fwversion);


	bytesread=read_from_usbl(getPowerLevel,buf);
	int power=atoi(buf);

	printf("Power level currently set at: %s\n\n",buf);

	if (power!=USBL_POWER_LEVEL)
	{
		bytesread=write_to_usbl(setPowerLevel,USBL_POWER_LEVEL,buf);
		usleep(DEFAULT_INTERVAL);
		bytesread=read_from_usbl(getPowerLevel,buf);
		//power=atoi(buf);
		printf("Power level set to: %s\n\n",buf);
	}


	bytesread=read_from_usbl(getWaveForm,buf);
	int wave=atoi(buf);

	printf("Wave form currently set at: %s\n\n",buf);

	if (wave!=USBL_WAVE_FORM)
	{
		bytesread=write_to_usbl(setWaveForm,USBL_WAVE_FORM,buf);
		usleep(DEFAULT_INTERVAL);
		bytesread=read_from_usbl(getWaveForm,buf);
		wave=atoi(buf);
		printf("Wave form set to: %s\n\n",buf);
	}



	bytesread=read_from_usbl(getUSBLRetryIM,buf);
	int retry=atoi(buf);

	printf("IM retry count currently set at: %s\n\n",buf);

	if (retry!=1)
	{
		bytesread=write_to_usbl(setUSBLRetryIM,USBL_WAVE_FORM,buf);
		usleep(DEFAULT_INTERVAL);
		bytesread=read_from_usbl(getUSBLRetryIM,buf);
		retry=atoi(buf);
		printf("IM retry count set to: %s\n\n",buf);
	}



	bytesread=read_from_usbl(getExtNotification,buf);
	int notif=atoi(buf);

	printf("Extended notification currently set at: %s\n\n",buf);

	if (notif!=USBL_EXT_NOTIFICATION)
	{
		bytesread=write_to_usbl(setExtNotification,USBL_EXT_NOTIFICATION,buf);
		usleep(DEFAULT_INTERVAL);
		bytesread=read_from_usbl(getExtNotification,buf);
		notif=atoi(buf);
		printf("Extended notification set to: %s\n\n",buf);
	}

	printf("USBL init complete.\n\n\n");

	return 0;
}


UsblLink::~UsblLink()
{
	
	if (cl != NULL)
	{
		cl->close();
		delete cl;
	}
	
	inLink->terminate();
	delete inLink;

	outLink->terminate();
	delete outLink;

	posLink->terminate();
	delete posLink;
	
}


void UsblLink::execute()
{
	if (device_mode==DEVICE_SIM) execute_sim();
	else if (device_mode==DEVICE_ACT) execute_act();
}


void UsblLink::execute_sim()
{
	timespec tSleep;
	tSleep.tv_sec=PINGER_SLEEP_SEC;
	tSleep.tv_nsec=PINGER_SLEEP_NSEC;


	while(running)
	{
		USBL_sim_packet msg;
		int ret;
		do{
			ret=inLink->recv_message((char*)&msg);
			if (ret>0)
			{
				strcpy(msgIn.data,msg.data);
				status(RECEIVED);
				sequence++;
				if (sequence>'z') sequence='0';
			}
		}while(ret>0);

		USBLpos_packet pos;
		do{
			ret=posLink->recv_message((char*)&pos);
			if (ret>0)
			{
				pos_x=((double)(pos.x))/USBLpos_factor;
				pos_y=((double)(pos.y))/USBLpos_factor;
				pos_z=((double)(pos.z))/USBLpos_factor;

				//printf("********* x: %lf   y: %lf   z: %lf\n",pos_x,pos_y,pos_z);
			}
		}while(ret>0);

		nanosleep(&tSleep,NULL);
	}
}

void UsblLink::execute_act()
{
	timespec tSleep;
	tSleep.tv_sec=0;
	tSleep.tv_nsec=100000000;



	int bytesread=0;
	char out[BUF_SIZE];
	char fullString[BUF_SIZE];

	char ts_buf[100];

	while(running)
	{

		bytesread=cl->read(out, BUF_SIZE);
		if (bytesread>0)
		{
			out[bytesread]='\0';
			strcat(fullString,out);
			//printf("fullString: %s\n",fullString);

			char dataBuf[100];


			char *sendend=strstr(fullString,"SENDEND"); //grab send timeStamp
			if (sendend!=NULL && status()==SENDING)
			{
				int j=0,k=0;
				for(int i=0;i<3;)
					if (sendend[j++]==',') i++;

				while(sendend[j]!=',')
					ts_buf[k++]=sendend[j++];
				ts_buf[k]='\0';
				msgOut.timeStamp=strtoul(ts_buf,NULL,10);
			}



			char *recvstart=strstr(fullString,"RECVSTART"); //grab reception timeStamp
			if (recvstart!=NULL && status()==RECEIVE)
			{
				status(RECEIVING);
			}



			char *recvend=strstr(fullString,"RECVEND"); //grab reception timeStamp
			if (recvend!=NULL && status()==RECEIVING)
			{
				int j=0,k=0;
				for(int i=0;i<1;)
					if (recvend[j++]==',') i++;

				while(recvend[j]!=',')
					ts_buf[k++]=recvend[j++];
				ts_buf[k]='\0';
				msgIn.timeStamp=strtoul(ts_buf,NULL,10);


				double t=msgIn.timeStamp - msgOut.timeStamp;
				if (t<0.0) t+=4294967296;
				t/=1000000.0;
				//printf("-------  ti: %ld    tf: %ld     t:%lf\n",msgOut.timeStamp , msgIn.timeStamp, t);


				range=t*1500.0;
			}




			char *recvim=strstr(out,"RECVIM");
			if (recvim!=NULL && status()==RECEIVING)
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

				//if (dataBuf[0]==sequence)
				{
					strcpy(msgIn.data,dataBuf+1);
					status(RECEIVED);
					sequence++;
					if (sequence>'z') sequence='0';
				}
				//else status(SEND);

				//printf("Message: %s   ts: %u\n",m->data,m->timeStamp);
			}

			char *deliveredim=strstr(out,"DELIVEREDIM");
			if (deliveredim!=NULL && status()==SENDING)
			{
				fullString[0]='\0';
				status(SENT);
			}

			char *failedim=strstr(out,"FAILEDIM");
			if (failedim!=NULL && status()==SENDING)
			{
				fullString[0]='\0';
				status(SEND);
			}

			char *cancelledim=strstr(out,"CANCELLEDIM");
			if (cancelledim!=NULL && status()==SENDING)
			{
				fullString[0]='\0';
				status(SEND);
			}


			char *error=strstr(out,"ERROR");
			if (error!=NULL && status()==SENDING)
			{
				fullString[0]='\0';
				status(SEND);
			}


			char *recvfailed=strstr(out,"RECVFAILED");
			if (recvfailed!=NULL)
			{
				fullString[0]='\0';
			}




			char *usbllong=strstr(out,"USBLLONG");
			if (usbllong!=NULL)
			{
				int remote_address;
				double curr_time,measurement_time,x,y,z,n,e,d,roll,pitch,yaw,propagation_time,rssi,integrity,accuracy;
				sscanf(usbllong,"USBLLONG,%lf,%lf,%d,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n"
						,&curr_time,&measurement_time,&remote_address,&x,&y,&z,&n,&e,&d,&roll,&pitch,&yaw,&propagation_time,&rssi,&integrity,&accuracy);

				double xr,yr,zr;
				xRot(M_PI,n,e,d,xr,yr,zr);
				n=xr; e=yr; d=zr;

				pos_x=n; pos_y=e; pos_z=zr;
				//printf("n: %lf   e: %lf   d: %lf\n",n,e,d);
			}



		}

		nanosleep(&tSleep,NULL);
	}
}



char* UsblLink::receiveUsblMessage()
{
	return msgIn.data;
}


void UsblLink::sendUsblMessage(char* m)
{
	if (status()==SEND)
	{
		status(SENDING);

		char buf[BUF_SIZE];
		//unsigned int dataLen=strlen(m);
		strcpy(msgOut.data,m);

		char msg[BUF_SIZE],xs[100],ys[100],zs[100];
		long xl,yl,zl;

		xl=(long)(fabs(pos_x)*100.0);
		sprintf(xs,"+%li",xl);
		if (pos_x<0.0) xs[0]='-';

		yl=(long)(fabs(pos_y)*100.0);
		sprintf(ys,"+%li",yl);
		if (pos_y<0.0) ys[0]='-';

		zl=(long)(fabs(pos_z)*100.0);
		sprintf(zs,"+%li",zl);
		if (pos_z<0.0) zs[0]='-';

		/*
		strcpy(zs,"+000000");
		zl=(long)(fabs(pos_z)*100.0);
		sprintf(ss,"%li",zl);
		len=strlen(ss);
		if (pos_z<0.0) zs[0]='-';
		zs[7-len]='\0';
		strcat(zs,ss);
*/

		sprintf(msg,"%c%s%s%s,%s",sequence,xs,ys,zs,m);
		unsigned int dataLen=strlen(msg);

		//printf("xs: %s   ys: %s   zs: %s\n",xs,ys,zs);
		//printf("msg: %s   %d\n",msg, dataLen);


		sprintf(buf,"AT*SENDIM,%d,%d,ack,%s\n",dataLen,PINGER_ADDR,msg);
		//printf("Sending: %s\n",buf);
		//cl->write(buf,strlen(buf));

		if (device_mode==DEVICE_ACT) sendUsblMessage_act(buf);
		else if (device_mode==DEVICE_SIM) sendUsblMessage_sim(msg,dataLen);
	}
}

void UsblLink::sendUsblMessage_act(char* buf)
{
	cl->write(buf,strlen(buf));

}

void UsblLink::sendUsblMessage_sim(char* buf,int bufLength)
{
	if (rand() % 100 != 0) outLink->send_message(buf,bufLength);
	status(SENT);
}



int UsblLink::read_from_usbl(char const *cmd,char *out)
{
	char command[BUF_SIZE];
	uint32_t bytesread;
	strcpy(command,cmd);
	strcat(command,"\n");
	cl->write(command,strlen(command));
	usleep(DEFAULT_INTERVAL);
	bytesread= cl->read(out, BUF_SIZE);
	out[bytesread]='\0';
	return bytesread;
}


int UsblLink::write_to_usbl(char const *cmd,int value,char *out)
{
	char command[BUF_SIZE];
	uint32_t bytesread;
	sprintf(command,"%s%d\n",cmd,value);
	cl->write(command,strlen(command));
	usleep(DEFAULT_INTERVAL);
	bytesread=cl->read(out, BUF_SIZE);
	out[bytesread]='\0';
	return bytesread;
}



void* start_UsblLink(void *arg)
{
	UsblLink* pThread=(UsblLink*)((ThreadInfo*)arg)->pThread;
	pThread->execute();
	return NULL;
}

