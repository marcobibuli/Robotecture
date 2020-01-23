/*
 * CommLink.h
 *
 *  Created on: Nov 9, 2016
 *      Author: mx
 */

#ifndef NETWORK_COMMLINK_H_
#define NETWORK_COMMLINK_H_



#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <vector>
#include <semaphore.h>

#include "../generic/custom_types.h"
#include "../generic/RobotThread.h"
#include "Message.h"
#include "MessageNumberGenerator.h"

#define MAX_BUFFER_LENGTH 64000

enum CommMode
{
	HARD_ACK=0,
	OVERRIDE,
	UDP_PURE,
};


enum LinkLevel
{
	NORMAL=0,
	WARNING,
	LOST
};


enum LinkProtocol
{
	IDLE=0,
	WAITING_ACK,
	RETRANSMIT,
	SEND_ACK
};


/*
enum LinkProtocol
{
	IDLE = 0,
	RECEIVE,
	RETRANSMIT
};
*/

struct LinkState
{
	LinkLevel linkLevel;
	LinkProtocol linkProtocol,sendState,recvState;
	Message lastMessageSent;
	Message lastMessageReceived;
};

struct Sender
{
	int32 ip;
	int32 port;
	int64 lastAckId;
};

class CommLink:public RobotThread
{
	private:
		int sock;
		struct sockaddr_in localAddr;
		struct sockaddr_in remoteAddr;
		uint16 localport;
		uint16 remoteport;
		char remoteIpAddress[64];
		char localIpAddress[64];
		LinkState linkState;
		std::vector<Message*> send_message_vector;
		std::vector<Message*> recv_message_vector;
		std::vector<Message*> recv_packets;
		std::vector<Sender*> senders;
		CommMode commMode;
		ssize_t messageLength;
		MessageNumberGenerator mng;
		struct sockaddr_in localAddr_for_message;

		int64 lastAckId;

		sem_t transmMutex,receiveMutex;

		int close();


	public:
		CommLink(const char *name, CommMode mode );
		~CommLink();

		int open( char *localIp , uint16 localPort , char *remoteIp=NULL , uint16 remotePort=-1 );
		
		int enable_blocking();
		int disable_blocking();

		ssize_t send(char *buf,int bufLen,int flags=0);
		ssize_t sendto(struct sockaddr_in addr,char *buf,int bufLen,int flags=0);
		ssize_t recv(char* buf,int bufLen,int flags=0);

		//int send_message(Message m);
		ssize_t send_message(char *buf,ssize_t bufLength,bool htonl_conversion=false);

		ssize_t recv_message(char *buf,bool ntohl_conversion=false);

		ssize_t push_message(char *buf,ssize_t bufLength);

		bool set_sender(int64 id,int32 ip,int32 port);

		void execute();
		void execute_hard_ack();
		void execute_override();
		void execute_udp_pure();

		LinkLevel getLinkLevel()const{return linkState.linkLevel;}

		void flush_buffer();

};

void* start_comm_link(void *arg);


#endif /* NETWORK_COMMLINK_H_ */
