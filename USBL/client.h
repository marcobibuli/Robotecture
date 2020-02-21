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

#ifndef CLIENT_H
#define CLIENT_H

#include "streamsocket.h"

const bool CLIENT_CONNECT_NOT_BLOCKING =false;
const bool CLIENT_CONNECT_BLOCKING     =true;

class Client: public StreamSocket
   { 
   private:
      char mIpServerAddress[IP_ADDRESS_MAX_SIZE];
      bool mBlockingSocketOnConnect;
      int m_client_open(void);
   public:
      Client(void);
      ~Client(void);
      int open(const char* ipServerAddr,int16 serverTcpPort,
               bool blockingSocketOnConnect=CLIENT_CONNECT_BLOCKING);
      int reopen(void);
      int close(void);
   };

#endif // CLIENT_H
