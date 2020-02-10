/*
 * Message.h
 *
 *  Created on: Nov 15, 2016
 *      Author: mx
 */

#ifndef NETWORK_MESSAGE_H_
#define NETWORK_MESSAGE_H_

#include <stdio.h>
#include <string>
#include <arpa/inet.h>
#include "../generic/custom_types.h"


struct Message
{
	private:
		int64 *pId;
		int32 *pIp;
		int32 *pPort;

		void initMessage(int64 nid,int32 addr,int p,ssize_t length)
		{
			dataLength = length;
			packetLength = sizeof(*pId) + sizeof(*pIp) + sizeof(*pPort) + dataLength;
			packet = new char[packetLength];
			pId = (int64*)packet;
			pIp = (int32*)(packet + sizeof(*pId));
			pPort = (int32*)(packet + sizeof(*pId) + sizeof(*pIp));
			*pId = nid;
			*pIp = addr;
			*pPort = p;
		}

	public:
		char *packet;
		ssize_t packetLength;
		ssize_t dataLength;

		Message()
		{
			initMessage(-1,0,0,0);
		}


		Message(ssize_t length)
		{
			initMessage(-1,0,0,length);
		}


		Message(int64 id,ssize_t length)
		{
			initMessage(id,0,0,length);
		}


		Message(int64 id,int32 ip,int32 port,char *buf,ssize_t length)
		{
			initMessage(id,ip,port,length);

			for(ssize_t i=dataPosition();i<packetLength;i++)
			{
				packet[i] = buf[i-dataPosition()];
			}
		}


		Message(const Message &m)
		{
			initMessage(m.id(),m.ip(),m.port(),m.dataLength);

			for(ssize_t i=0;i< packetLength;i++)
			{
				packet[i] = m.packet[i];
			}
		}


		~Message()
		{
			delete[] packet;
		}


		ssize_t const size(){return packetLength;}
		ssize_t const dataSize() { return dataLength; }

		char* dataPointer(){return (char*)(&packet[dataPosition()]);}

		int64 id()const{return *pId;}
		void id(int64 nid){*pId=nid;}

		int32 port()const{return *pPort;}
		void port(int32 p){*pPort=p;}

		int32 ip()const{return *pIp;}
		void ip(int32 addr){*pIp=addr;}

		ssize_t idPosition()const{return 0;}
		ssize_t ipPosition()const{return sizeof(*pId);}
		ssize_t portPosition()const{return (sizeof(*pId)+sizeof(*pIp));}
		ssize_t dataPosition()const{return (sizeof(*pId)+sizeof(*pIp)+sizeof(*pPort));}

		void resizePacket(ssize_t ns){ packetLength =ns; dataLength = packetLength -(sizeof(*pId) + sizeof(*pIp) + sizeof(*pPort));}
		void resizeData(ssize_t ns) { dataLength = ns;  packetLength = dataLength +(sizeof(*pId) + sizeof(*pIp) + sizeof(*pPort)); }
		
		
		void packet_ntohl() { uint32* ptr = (uint32*)packet; for (int i = 0; i < (packetLength / sizeof(uint32)); i++, ptr++) *ptr = ntohl(*ptr); }
		void packet_htonl() { uint32* ptr = (uint32*)packet; for (int i = 0; i < (packetLength / sizeof(uint32)); i++, ptr++) *ptr = htonl(*ptr); }


		void copyDataTo(char *dest)
		{
			for(ssize_t i=dataPosition();i< packetLength;i++)
				dest[i-dataPosition()]= packet[i];
		}

		Message operator=(Message &m)
		{
			delete[] packet;

			initMessage(m.id(),m.ip(),m.port(),m.dataLength);

			for(ssize_t i=0;i< packetLength;i++)
			{
				packet[i] = m.packet[i];
			}

			return *this;
		}
};



#endif /* NETWORK_MESSAGE_H_ */
