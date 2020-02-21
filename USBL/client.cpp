/*=========================================================================*\
||                                                                         ||
||                     Code written by: Gabriele Bruzzone                  ||
||                                                                         ||
\*=========================================================================*/

// Release 1.1, 10/03/2005
// Release 1.0, 15/02/2005

//---------------------------------------------------------------------------
//
// client.h
//
//---------------------------------------------------------------------------

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "my_types.h"
#include "debug.h"
#include "streamsocket.h"
#include "client.h"


//---------------------------------------------------------------------------
//
// Client::Client(void)
//
//---------------------------------------------------------------------------
Client::Client(void)
{
 // do nothing
 printf("Client::Client - construction OK\n");
}


//---------------------------------------------------------------------------
//
// Client::~Client(void)
//
//---------------------------------------------------------------------------
Client::~Client(void)
{
 // do nothing
 printf("Client::~Client - destruction OK\n");
}


//---------------------------------------------------------------------------
//
// int Client::m_client_open(void)
//
//---------------------------------------------------------------------------
int Client::m_client_open(void)
{
struct sockaddr_in sockBuff;

 printf("Client::open - server IP address: %s\n",mIpServerAddress);
 printf("Client::open - server TCP port: %hd\n",mTcpPort);
 printf("Client::open - opening\n");

 while(true)
    {
    mSockId=socket(AF_INET,SOCK_STREAM,0);
    if(mSockId<0)
       {
       printf("Client::open error - socket (%d)\n",errno);
       mSockId=0;
       return(-1);
       }

    sockBuff.sin_family=AF_INET;
    // family chosen 
    if((sockBuff.sin_addr.s_addr=inet_addr(mIpServerAddress))<0)
       {
       printf("Client::open error - inet_addr");
       ::close(mSockId);
       mSockId=0;
       return(-1);
       }
    // obtained IP address is the server address

    sockBuff.sin_port=htons((u_short)mTcpPort);
    // the port on which the server is waiting

    printf("Client::open - attempting connection with server: %s on port: %hd...\n",mIpServerAddress,mTcpPort);

    if(connect(mSockId,(struct sockaddr *)&sockBuff,sizeof(struct sockaddr_in))<0)
       {
       if(mBlockingSocketOnConnect)
          {
          sleep(1);
          ::close(mSockId);
          mSockId=0;
          continue; // continue infinite loop
          }
       else  // if !blockingSocketOnConnect
          {
          printf("Client::open error - connect (%d)\n",errno);
          ::close(mSockId);
          mSockId=0;
          return(-1);
          }
       }
    else // if successful connection
       {
       break; // exit from infinite loop
       }
    }

 struct linger linger;
 linger.l_onoff=1;
 linger.l_linger=0;
 if(setsockopt(mSockId,SOL_SOCKET,SO_LINGER,(char *)&linger,sizeof(linger))<0)
    {
    printf("Client::open error - setsockopt (SO_LINGER option) (%d)\n",errno);
    ::close(mSockId);
    mSockId=0;
    return(-1);
    }

 printf("Client::open - opening OK\n");

 return(0);
}


//---------------------------------------------------------------------------
//
// int Client::open(const char* ipServerAddress,int16 serverTcpPort,
//                  bool blockingSocketOnConnect)
//
//---------------------------------------------------------------------------
int Client::open(const char* ipServerAddress,int16 serverTcpPort,
                 bool blockingSocketOnConnect)
{

 strcpy(mIpServerAddress,ipServerAddress);
 mTcpPort=serverTcpPort;
 mBlockingSocketOnConnect=blockingSocketOnConnect;
 
 return(m_client_open());
} 


//---------------------------------------------------------------------------
//
// int Client::reopen(void)
//
//---------------------------------------------------------------------------
int Client::reopen(void)
{
 require(mSockId==0);
 return(m_client_open());
}


//---------------------------------------------------------------------------
//
// int Client::close(void)
//
//---------------------------------------------------------------------------
int Client::close(void)
{

 require(mSockId!=0);

 printf("Client::close - closing\n");

 if(::close(mSockId)<0)
    {
    printf("Client::close error - close (%d)",errno);
    return(-1);
    }

 mSockId=0;

 printf("Client::close - closing OK\n");

 return(0);
}


