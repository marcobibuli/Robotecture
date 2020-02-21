/*=========================================================================*\
||                                                                         ||
||                     Code written by: Gabriele Bruzzone                  ||
||                                                                         ||
\*=========================================================================*/

// Release 1.1, 10/03/2005
// Release 1.0, 15/02/2005

//---------------------------------------------------------------------------
//
// streamsocket.h
//
//---------------------------------------------------------------------------

#ifndef STREAMSOCKET_H
#define STREAMSOCKET_H

const int IP_ADDRESS_MAX_SIZE=64;

#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>

#include "my_types.h"

class StreamSocket
   {
   protected:
      int16 mTcpPort;
      int mSockId;

   public:
      StreamSocket(void);
      virtual ~StreamSocket(void)=0;
      virtual int close(void)=0;
      virtual int reopen(void)=0;

      int write(const char *pOutputBuffer, int bufferSize);
      int send(const char *pOutputBuffer, int bufferSize, int flags);
            
      int read(char* pInputBuffer,int bufferSize);
      int recv(char* pInputBuffer,int bufferSize,int flags);
 
      int enable_blocking(void);
      int disable_blocking(void);
 
      int enable_no_delay(void);
      int disable_no_delay(void);
    };

#endif // STREAMSOCKET_H

