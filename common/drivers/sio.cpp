/*=========================================================================*\
||                                                                         ||
||                  Consiglio Nazionale delle Ricerche                     ||
||                                                                         ||
\*=========================================================================*/

// Release 1.1, 10/06/2004 Added some minor modifications
// Release 1.0, 05/09/2003

//---------------------------------------------------------------------------
//
// sio.cpp
//
//---------------------------------------------------------------------------

#include <errno.h>	
#include <fcntl.h>	
#include <stdio.h>	
#include <string.h>							  
#include <termios.h>
#include <unistd.h>


#include "sio.h"

//---------------------------------------------------------------------------
//
// Sio::Sio(void)
//
//---------------------------------------------------------------------------
Sio::Sio(void)
{
 mFdCom=0;
 printf("Sio::Sio - construction OK\n");
}


//---------------------------------------------------------------------------
//
// int Sio::init(const char* device,uint32 baudRate,uint32 dataBits,
//               uint32 stopBits,uint32 parityEnable,uint32 parityType,
//               uint32 receivingEnable,uint32 noBlockingFlag)
//
//---------------------------------------------------------------------------
int Sio::init(const char* device,uint32 baudRate,uint32 dataBits,
              uint32 stopBits,uint32 parityEnable,uint32 parityType,
              uint32 receivingEnable,uint32 noBlockingFlag)
{


 switch(baudRate)
    {
    case B50:
    case B75:
    case B110:
    case B134:
    case B150:
    case B200:
    case B300:
    case B600:
    case B1200:
    case B1800:
    case B2400:
    case B4800:
    case B9600:
    case B19200:
    case B38400:
    case B57600:
    case B115200:
    case B230400:
       // all ok
    break;
    default:
       printf("Sio::init error - wrong baud rate: %lu\n",baudRate);
       return(-1);
    break;
    }

 switch(dataBits)
    {
    case CS5:
    case CS6:
    case CS7:
    case CS8:
       // all ok
    break;
    default:
       printf("Sio::init error - wrong data bits: %lu\n",dataBits);
       return(-1);
    break;
    }

 switch(stopBits)
    {
    case 0:
    case CSTOPB:
       // all ok
    break;
    default:
       printf("Sio::init error - wrong stop bits: %lu\n",stopBits);
       return(-1);
    break;
    }

 switch(parityEnable)
    {
    case 0:
    case PARENB:
       // all ok
    break;
    default:
       printf("Sio::init error - wrong parity enable bit: %lu\n",parityEnable);
       return(-1);
    break;
    }

 switch(parityType)
    {
    case 0:
    case PARODD:
       // all ok
    break;
    default:
       printf("Sio::init error - wrong parity type: %lu\n",parityType);
       return(-1);
    break;
    }

 switch(receivingEnable)
    {
    case 0:
    case CREAD:
       // all ok
    break;
    default:
       printf("Sio::init error - wrong receiving enable: %lu\n",receivingEnable);
       return(-1);
    break;
    }

 switch(noBlockingFlag)
    {
    case 0:
    case O_NONBLOCK:
       // all ok
    break;
    default:
       printf("Sio::init error - wrong no blocking flag: %lu\n",noBlockingFlag);
       return(-1);
    break;
    }

 if((mFdCom=open(device,O_RDWR|noBlockingFlag))<0)
    {				  
    printf("Sio::init error - open %s\n",strerror(errno));
    return(-1);
    }  			  

 if(tcflush(mFdCom,TCIOFLUSH)<0)
    {
    printf("Sio::init error - flush %s\n",strerror(errno));
    return(-1);
    }

 tcgetattr(mFdCom,&mOldTio);	  
									  
 memset(&mNewTio,0,sizeof(mNewTio));		

 // set iflag values for raw management
 mNewTio.c_iflag=IGNBRK|IGNPAR;

 // set cflag values
 mNewTio.c_cflag=dataBits|stopBits|parityEnable|
                 parityType|receivingEnable|CLOCAL;
 mNewTio.c_cc[VMIN] =1; // blocking read until 1 char received
 mNewTio.c_cc[VTIME]=0; // inter-character timer not used

 // set output speed
 if(cfsetospeed(&mNewTio,baudRate)<0)
    {
    printf("Sio::init error - cfsetospeed %s\n",strerror(errno));
    return(-1);
    } 

 // set input speed
 if(cfsetispeed(&mNewTio,baudRate)<0)
    {
    printf("Sio::init error - cfsetospeed %s\n",strerror(errno));
    return(-1);
    } 

 tcsetattr(mFdCom,TCSANOW,&mNewTio);					  

 printf("Sio::init - initialization OK\n");

 return(0);
}


//---------------------------------------------------------------------------
//
// int Sio::terminate(void)
//
//---------------------------------------------------------------------------
int Sio::terminate(void)
{


 tcsetattr(mFdCom,TCSANOW,&mOldTio);

// dprintf("Sio::terminate - termination OK\n");

 mFdCom=0;
 
 return(0);
}

//---------------------------------------------------------------------------
//
// Sio::~Sio(void)
//
//---------------------------------------------------------------------------
Sio::~Sio(void)
{

// dprintf("Sio::Sio - destruction OK\n");
}
