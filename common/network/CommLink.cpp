/*
 * CommLink.cpp
 *
 *  Created on: Nov 9, 2016
 *      Author: mx
 */


#include "CommLink.h"



CommLink::CommLink(const char *name, CommMode mode ):
	RobotThread(name,SCHED_FIFO,COMM_LINK_THREAD_PRIORITY,start_comm_link)
{
	sock=0;
	localport=0;
	remoteport=0;

	linkState.linkLevel=NORMAL;
	linkState.linkProtocol=IDLE;

	commMode=mode;

	lastAckId=-1;

	messageLength=0;

	sem_init(&transmMutex,0,1);
	sem_init(&receiveMutex,0,1);
}


CommLink::~CommLink(void)
{
	if(sock!=0)
    {
		::close(sock);
		/*
		if(::close(sock)<0)
		{
			printf("CommLink::~CommLink - Close error\n");
		}
		*/
    }

}


int CommLink::open( char *localIp , uint16 localPort , char *remoteIp , uint16 remotePort )
{
	localport=localPort;

	remoteIpAddress[0]=0;

	if (remoteIp!=NULL) strcpy(remoteIpAddress,remoteIp);
	strcpy(localIpAddress,localIp);

	//printf("%s:  %s %d   %s %d\n",threadName,localIpAddress,local,remoteIpAddress,remote);

	if (remotePort !=-1) remoteport= remotePort;
	else remoteport= localPort;

	memset((char*)&localAddr_for_message,0,sizeof(struct sockaddr_in));
	localAddr_for_message.sin_family=AF_INET;
	localAddr_for_message.sin_port=htons((uint16)this->localport);
	localAddr_for_message.sin_addr.s_addr=inet_addr(this->localIpAddress);

	memset((char*)&remoteAddr,0,sizeof(struct sockaddr_in));
	remoteAddr.sin_family=AF_INET;
	remoteAddr.sin_port=htons((uint16)this->remoteport);
	remoteAddr.sin_addr.s_addr=inet_addr(this->remoteIpAddress);

	//printf("local: %ld    remote: %ld\n", *((long*)(&localAddr_for_message.sin_addr)), *((long*)(&remoteAddr.sin_addr)));

	if((sock=socket(AF_INET,SOCK_DGRAM,0))==-1)
	{
		printf("CommLink::open - socket call error (%d)\n",errno);
		return(-1);
	}

	memset((char*)&localAddr,0,sizeof(struct sockaddr_in));
	localAddr.sin_family=AF_INET;
	localAddr.sin_port=htons((uint16)this->localport);
	localAddr.sin_addr.s_addr=htonl(INADDR_ANY);

	if(bind(sock,(struct sockaddr*)&localAddr,sizeof(struct sockaddr_in))==-1)
	{
		printf("%s - CommLink::open - bind call error (%s)\n",threadName,strerror(errno));
		::close(sock);
		sock=0;
		return(-1);
	}

	disable_blocking();

	//printf("local: %ld    remote: %ld\n", *((long*)(&localAddr_for_message)), *((long*)(&remoteAddr)));

	return 0;
}


int CommLink::close()
{
	/*
	if(::close(sock)<0)
	{
		printf("CommLink::Close - close error\n");
		return -1;
	}
	*/

	if(sock!=0) ::close(sock);


	for(size_t i=0;i<send_message_vector.size();i++)
		if (send_message_vector[i]!=NULL) delete send_message_vector[i];
	send_message_vector.clear();

	for(size_t i=0;i<recv_message_vector.size();i++)
		if (recv_message_vector[i]!=NULL) delete recv_message_vector[i];
	recv_message_vector.clear();

	for(size_t i=0;i<recv_packets.size();i++)
		if (recv_packets[i]!=NULL) delete recv_packets[i];
	recv_packets.clear();

	for(size_t i=0;i<senders.size();i++)
		if (senders[i]!=NULL) delete senders[i];
	senders.clear();

	return 0;
}


int CommLink::enable_blocking()
{
	int flags=fcntl(sock,F_GETFL,0);
	if(flags==-1)
	{
		printf("CommLink::enable_blocking - fcntl call error (%d)\n",errno);
		return -1;
	}
	if(fcntl(sock,F_SETFL,flags&(~O_NONBLOCK))==-1)
	{
		printf("CommLink::enable_blocking - fcntl call error (%d)\n",errno);
		return -1;
	}

	return(0);
}


int CommLink::disable_blocking(void)
{
	int flags=fcntl(sock,F_GETFL,0);
	if(flags==-1)
	{
		printf("CommLink::disable_blocking - fcntl call error (%d)\n",errno);
		return -1;
	}
	if(fcntl(sock,F_SETFL,flags|O_NONBLOCK)==-1)
	{
		printf("CommLink::disable_blocking - fcntl call error (%d)\n",errno);
		return -1;
	}

	return(0);
}



ssize_t CommLink::send(char* buf,int bufLen,int flags)
{
	return(::sendto(sock,buf,bufLen,flags,(struct sockaddr*)& (remoteAddr),sizeof(struct sockaddr_in)));
}


ssize_t CommLink::sendto(struct sockaddr_in addr,char* buf,int bufLen,int flags)
{
	return(::sendto(sock,buf,bufLen,flags,(struct sockaddr*)&addr,sizeof(struct sockaddr_in)));
}


void CommLink::flush_buffer()
{
	char buf[1024];
	int ret;

	do{
		ret=recv(buf,1024,0);
	}while(ret>0);

}


ssize_t CommLink::recv(char* buf,int bufLen,int flags)
{
	int tmpLength;
	struct sockaddr_in tmpSockAddr;
	int returnValue;

	tmpLength=sizeof(struct sockaddr_in);
	returnValue=::recvfrom(sock,buf,bufLen,flags,(struct sockaddr*)&tmpSockAddr,(socklen_t*)&tmpLength);

	return(returnValue);
}


ssize_t CommLink::send_message(char *buf,ssize_t bufLength,bool htonl_conversion)
{
	Message *m=new Message(mng.get(),*((long*)(&localAddr_for_message.sin_addr)),localport,buf,bufLength);

	if (htonl_conversion) m->data_htonl();

	sem_wait(&transmMutex);

		send_message_vector.push_back(m);

	sem_post(&transmMutex);

	return 0;
}


ssize_t CommLink::push_message(char *buf,ssize_t bufLength)
{
	Message *m=new Message(mng.get(),*((long*)(&localAddr_for_message.sin_addr)),localport,buf,bufLength);

	sem_wait(&receiveMutex);

		recv_message_vector.push_back(m);

	sem_post(&receiveMutex);

	return 0;
}


ssize_t CommLink::recv_message(char *buf,bool ntohl_conversion)
{
	ssize_t ret=0;

	sem_wait(&receiveMutex);
		if (!recv_message_vector.empty())
		{
			//printf("Received dato: %lli\n",*((int64*)recv_message_vector[0]->dataPointer()));

			if (ntohl_conversion) recv_message_vector[0]->data_ntohl();

			recv_message_vector[0]->copyDataTo(buf);
			ret=recv_message_vector[0]->dataLength;
			recv_message_vector.erase(recv_message_vector.begin());

		}

		if (recv_message_vector.empty())
		{
			lastAckId=-1;
		}
	sem_post(&receiveMutex);

	return ret;
}





void CommLink::execute()
{
	flush_buffer();
	switch (commMode)
	{
		case HARD_ACK: execute_hard_ack();
					   break;

		case OVERRIDE: execute_override();
					   break;

		case UDP_PURE: execute_udp_pure();
					   break;

	};
}


bool CommLink::set_sender(int64 id,int32 ip,int32 port)
{
	size_t i;
	bool found=false;
	bool to_be_stored=true;

	for(i=0;i<senders.size() && !found;)
		if (senders[i]->ip==ip && senders[i]->port==port) found=true;
		else i++;

	if (!found)
	{
		Sender *s=new Sender;
		s->lastAckId=id;
		s->ip=ip;
		s->port=port;

		senders.push_back(s);

		to_be_stored=true;
	}
	else
	{
		if (id>senders[i]->lastAckId || id==1)
		{
			senders[i]->lastAckId=id;
			to_be_stored=true;
		}
		else to_be_stored=false;
	}
	return to_be_stored;
}


void CommLink::execute_hard_ack()
{

	timespec tSleep;
	tSleep.tv_sec = COMM_LINK_SLEEP_SEC;
	tSleep.tv_nsec = COMM_LINK_SLEEP_NSEC;

	linkState.sendState = IDLE;
	linkState.recvState = IDLE;


	int wait_loop_counter = 0;
	int lost_loop_counter = 0;

	flush_buffer();

	while (running)
	{
		
		if (linkState.sendState == IDLE)
		{
			sem_wait(&transmMutex);
			if (!send_message_vector.empty())  // if queued message, then send
			{
				wait_loop_counter = 0;
				lost_loop_counter = 0;

				linkState.lastMessageSent = *(send_message_vector[0]);
				send_message_vector.erase(send_message_vector.begin());

				Message m(linkState.lastMessageSent);

				//printf("send - ip: %d   port: %d\n", m.ip(), m.port());
				//printf("data sent: %s     id: %lli\n", m.dataPointer(),m.id());
				
				//m.header_htonl();

				if (send(m.packet, m.size(), 0) <= 0)
					printf("CommLink::execute_hard_ack() - send error\n");

				linkState.sendState = WAITING_ACK;
			}
			sem_post(&transmMutex);
		}
		
		if (linkState.sendState == WAITING_ACK || linkState.recvState==IDLE)  //waiting ack or just receiving incoming messages
		{
			Message m(MAX_BUFFER_LENGTH);
			ssize_t byteReceived;
			bool received = false;

			while ((byteReceived = recv(m.packet, m.size(), 0)) > 0)
			{
				m.resizePacket(byteReceived);

				//m.header_ntohl();

				if (m.id() == linkState.lastMessageSent.id() && m.size()==16)  //ack message
				{
					//printf("ack recevived: %lli    %d\n", m.id(),m.size());
					linkState.sendState = IDLE;
					linkState.linkLevel = NORMAL;
					received = true;
				}
				else
				{
					//printf("data recevived: %s     id: %lli\n", m.dataPointer(),m.id());
					recv_packets.push_back(new Message(m));
					
					bool to_be_stored = set_sender(m.id(), m.ip(), m.port());

					if (to_be_stored)
					{
						sem_wait(&receiveMutex);
						recv_message_vector.push_back(new Message(m));
						sem_post(&receiveMutex);
					}

					linkState.recvState = SEND_ACK;
				}
			}

			if (linkState.sendState == WAITING_ACK && !received)
			{
				wait_loop_counter++;

				if (wait_loop_counter > COMM_LINK_WAITING_LOOPS)
				{
					wait_loop_counter = 0;
					linkState.sendState = RETRANSMIT;
					linkState.linkLevel = WARNING;
				}
			}
		}

		if (linkState.sendState == RETRANSMIT)
		{
			Message m(linkState.lastMessageSent);
			//m.header_htonl();
			//printf("%s    Retransmit id: %lli\n",threadName,m.id());

			if (send(m.packet, m.size(), 0) <= 0)
				printf("CommLink::execute_hard_ack() - Retransmit error  size: %d   %s\n", m.size(), this->threadName);

			lost_loop_counter++;
			linkState.sendState = WAITING_ACK;

			if (lost_loop_counter > COMM_LINK_LOST_LOOPS)
			{
				linkState.linkLevel = LOST;
			}
		}

		if (linkState.recvState == SEND_ACK)
		{
			Message m_ack;

			while (!recv_packets.empty())
			{
				m_ack.id(recv_packets[0]->id());
				int32 msg_ip = (recv_packets[0]->ip());
				int32 msg_port = recv_packets[0]->port();

				//printf("ack --- ip: %d   port: %d    id: %lli\n", msg_ip,msg_port,m_ack.id());

				struct sockaddr_in rem;
				memset((char*)&rem, 0, sizeof(struct sockaddr_in));
				rem.sin_family = AF_INET;
				rem.sin_port = htons((uint16)msg_port);
				rem.sin_addr.s_addr = (msg_ip);

				m_ack.ip(*((long*)(&localAddr_for_message.sin_addr)));
				m_ack.port(localport);

				recv_packets.erase(recv_packets.begin());

				//m_ack.header_htonl();
				sendto(rem, m_ack.packet, m_ack.size(), 0);
				linkState.recvState = IDLE;

				//printf("Sent ack\n");
			}
		}


		nanosleep(&tSleep, NULL);
	}

	close();
	terminated = true;
}

/*
void CommLink::execute_hard_ack_old()
{

	timespec tSleep;
	tSleep.tv_sec = COMM_LINK_SLEEP_SEC;
	tSleep.tv_nsec = COMM_LINK_SLEEP_NSEC;


	int wait_loop_counter = 0;
	int lost_loop_counter = 0;

	while (running)
	{

		// TRANSMISSION PART

		sem_wait(&transmMutex);
		if (!send_message_vector.empty() && (linkState.linkProtocol == IDLE ))
		{
			wait_loop_counter = 0;
			lost_loop_counter = 0;

			linkState.lastMessageSent = *(send_message_vector[0]);
			send_message_vector.erase(send_message_vector.begin());

			Message m(linkState.lastMessageSent);
			//printf("ip: %d   port: %d\n", m.ip(), m.port());
			m.header_htonl();

			

			//printf("sending: %lli\n",*((int64*)m.dataPointer()));

			if (send(m.packet, m.size(), 0) <= 0)
				printf("CommLink::execute_hard_ack() - send error\n");


			linkState.linkProtocol = WAITING_ACK;
		}

		sem_post(&transmMutex);


		if (linkState.linkProtocol == WAITING_ACK)
		{
			Message m_ack;

			ssize_t byteReceived = recv(m_ack.packet, m_ack.size(), 0);

			if (byteReceived == m_ack.size())
			{
				m_ack.header_ntohl();
				if (m_ack.id() == linkState.lastMessageSent.id())
				{
					linkState.linkProtocol = IDLE;
					linkState.linkLevel = NORMAL;
					//printf("Received ack\n");
				}
			}

			else
			{
				wait_loop_counter++;

				if (wait_loop_counter > COMM_LINK_WAITING_LOOPS)
				{
					wait_loop_counter = 0;
					linkState.linkProtocol = RETRANSMIT;
					linkState.linkLevel = WARNING;
				}
			}
		}

		if (linkState.linkProtocol == RETRANSMIT)
		{
			Message m(linkState.lastMessageSent);
			m.header_htonl();
			//printf("%s    Retransmit id: %lli\n",threadName,m.id());

			if (send(m.packet, m.size(), 0) <= 0)
				printf("CommLink::execute_hard_ack() - Retransmit error  size: %d   %s\n", m.size(), this->threadName);

			lost_loop_counter++;
			linkState.linkProtocol = WAITING_ACK;

			if (lost_loop_counter > COMM_LINK_LOST_LOOPS)
			{
				linkState.linkLevel = LOST;
			}
		}
		// END TRANSMISSION PART



		// RECEPTION PART

		if (linkState.linkProtocol == IDLE)
		{
			Message m(MAX_BUFFER_LENGTH);
			ssize_t byteReceived;

			while ((byteReceived = recv(m.packet, m.size(), 0)) > 0)
			{
				m.resizePacket(byteReceived);

				//printf("pre_ntohl   ip: %ld     port: %ld\n", m.ip(), m.port());

				m.header_ntohl();

				recv_packets.push_back(new Message(m));

				//bool to_be_stored = set_sender(m.id(), ntohl(m.ip()), m.port());
				bool to_be_stored = set_sender(m.id(), m.ip(), m.port());

				//printf("post_ntohl   ip: %ld     port: %ld\n",m.ip(),m.port());

				if (to_be_stored)
				{
					sem_wait(&receiveMutex);
					recv_message_vector.push_back(new Message(m));
					sem_post(&receiveMutex);
				}

				linkState.linkProtocol = SEND_ACK;
			}
		}

		if (linkState.linkProtocol == SEND_ACK)
		{
			Message m_ack;

			while (!recv_packets.empty())
			{
				m_ack.id(recv_packets[0]->id());
				int32 msg_ip = (recv_packets[0]->ip());
				int32 msg_port = recv_packets[0]->port();

				//printf("ack --- ip: %d   port: %d\n", msg_ip,msg_port);

				struct sockaddr_in rem;
				memset((char*)& rem, 0, sizeof(struct sockaddr_in));
				rem.sin_family = AF_INET;
				rem.sin_port = htons((uint16)msg_port);
				rem.sin_addr.s_addr = (msg_ip);

				m_ack.ip(*((long*)(&localAddr_for_message.sin_addr)));
				m_ack.port(localport);

				recv_packets.erase(recv_packets.begin());

				m_ack.header_htonl();
				sendto(rem, m_ack.packet, m_ack.size(), 0);
				linkState.linkProtocol = IDLE;

				//printf("Sent ack\n");
			}
		}

		// END RECEPTION PART

		nanosleep(&tSleep, NULL);
	}

	close();
	terminated = true;

}
*/


void CommLink::execute_override()
{
	/*
	timespec tSleep;
	tSleep.tv_sec=COMM_LINK_SLEEP_SEC;
	tSleep.tv_nsec=COMM_LINK_SLEEP_NSEC;


	int wait_loop_counter=0;
	int lost_loop_counter=0;

	while(running)
	{

		// TRANSMISSION PART

		sem_wait(&transmMutex);

		//if (strcmp(threadName,"NGC_tlm")==NULL) printf("%s     %d    %d\n", threadName, send_message_vector.empty(), linkState.linkProtocol);

		if(!send_message_vector.empty() && (linkState.linkProtocol==IDLE || linkState.linkProtocol== WAITING_ACK))
		{
			wait_loop_counter=0;
			lost_loop_counter=0;

			linkState.lastMessageSent=*(send_message_vector[0]);
			send_message_vector.erase(send_message_vector.begin());

			Message m(linkState.lastMessageSent);
			m.header_htonl();

			//printf("sending: %lli\n",*((int64*)m.dataPointer()));

			if (send(m.packet,m.size(),0)<=0)
				printf("CommLink::execute_override() - send error\n");


			linkState.linkProtocol=WAITING_ACK;
		}

		sem_post(&transmMutex);


		if (linkState.linkProtocol==WAITING_ACK)
		{
			Message m_ack;

			ssize_t byteReceived=recv(m_ack.packet,m_ack.size(),0);

			if (byteReceived==m_ack.size())
			{
				m_ack.header_ntohl();
				if (m_ack.id()==linkState.lastMessageSent.id())
				{
					linkState.linkProtocol=IDLE;
					linkState.linkLevel=NORMAL;
					//printf("Received ack\n");
				}
			}

			else
			{
				//printf("%s    loop_counter id: %d    \n", threadName, wait_loop_counter);
				wait_loop_counter++;

				if (wait_loop_counter>COMM_LINK_WAITING_LOOPS)
				{
					wait_loop_counter=0;
					linkState.linkProtocol=RETRANSMIT;
					linkState.linkLevel=WARNING;
				}
			}
		}

		if (linkState.linkProtocol==RETRANSMIT)
		{
			Message m(linkState.lastMessageSent);
			m.header_htonl();
			printf("Retransmit id: %lli\n",m.id());

			if (send(m.packet,m.size(),0)<=0)
				printf("CommLink::execute_override() - Retransmit error  size: %d   %s\n",m.size(),this->threadName);

			lost_loop_counter++;
			linkState.linkProtocol=WAITING_ACK;

			if (lost_loop_counter>COMM_LINK_LOST_LOOPS)
			{
				linkState.linkLevel=LOST;
			}
		}
		// END TRANSMISSION PART
		


		// RECEPTION PART

		if(linkState.linkProtocol==IDLE)
		{
			Message m(MAX_BUFFER_LENGTH);
			ssize_t byteReceived;

			while( (byteReceived=recv(m.packet,m.size(),0)) > 0 )
			{
				m.resizePacket(byteReceived);
				m.header_ntohl();

				recv_packets.push_back(new Message(m));

				bool to_be_stored=set_sender(m.id(),ntohl(m.ip()),m.port());

				if (to_be_stored)
				{
					sem_wait(&receiveMutex);
					recv_message_vector.push_back(new Message(m));
					sem_post(&receiveMutex);
				}

				linkState.linkProtocol=SEND_ACK;
			}
		}

		if(linkState.linkProtocol==SEND_ACK)
		{
			Message m_ack;

			while(!recv_packets.empty())
			{
				m_ack.id(recv_packets[0]->id());
				int32 msg_ip=(recv_packets[0]->ip());
				int32 msg_port=recv_packets[0]->port();

				struct sockaddr_in rem;
				memset((char*)&rem,0,sizeof(struct sockaddr_in));
				rem.sin_family=AF_INET;
				rem.sin_port=htons((uint16)msg_port);
				rem.sin_addr.s_addr=msg_ip;

				m_ack.ip(*((long*)(&localAddr_for_message.sin_addr)));
				m_ack.port(localport);

				recv_packets.erase(recv_packets.begin());

				m_ack.header_htonl();
				sendto(rem,m_ack.packet,m_ack.size(),0);
				linkState.linkProtocol=IDLE;
			}
		}

		// END RECEPTION PART

		nanosleep(&tSleep,NULL);
	}

	close();
	terminated=true;
	*/
}


void CommLink::execute_udp_pure()
{
	timespec tSleep;
	tSleep.tv_sec=COMM_LINK_SLEEP_SEC;
	tSleep.tv_nsec=COMM_LINK_SLEEP_NSEC;

	while(running)
	{
		sem_wait(&transmMutex);
		if(!send_message_vector.empty())
		{
			linkState.lastMessageSent=*(send_message_vector[0]);
			send_message_vector.erase(send_message_vector.begin());
			Message m(linkState.lastMessageSent);
			//m.header_htonl();
/*
			int64 ddd=*((int64*)linkState.lastMessageSent.dataPointer());
			printf("Sent.  %lli  %li  %li  %lli\n",linkState.lastMessageSent.id(),linkState.lastMessageSent.ip(),linkState.lastMessageSent.port(),ddd);
*/
			//int zzz = send(m.frame, m.size(), 0);
			int zzz = send(m.dataPointer(), m.dataLength, 0);
			if (zzz <= 0)
				printf("CommLink::execute_udp_pure() - send error\n");
			//else printf("Sent: %d\n", zzz);
			
		}
		sem_post(&transmMutex);


		ssize_t byteReceived;
		Message m(MAX_BUFFER_LENGTH);
		//while( (byteReceived=recv(m.frame,m.size(),0)) >0 )
		while ((byteReceived = recv(m.dataPointer(), m.dataSize(), 0)) > 0)
		{
			//printf("arriva roba... %d\n", byteReceived);
			m.resizeData(byteReceived);
			//m.header_ntohl();

			sem_wait(&receiveMutex);
			recv_message_vector.push_back(new Message(m));
			sem_post(&receiveMutex);
		}

		nanosleep(&tSleep,NULL);
	}

	close();
	terminated=true;
}


void* start_comm_link(void *arg)
{
	CommLink* pThread=(CommLink*)((ThreadInfo*)arg)->pThread;
	pThread->execute();
	return NULL;
}

