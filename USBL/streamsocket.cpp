/*=========================================================================*\
||                                                                         ||
||                     Code written by: Gabriele Bruzzone                  ||
||                                                                         ||
\*=========================================================================*/

// Release 1.1, 10/03/2005
// Release 1.0, 15/02/2005

//---------------------------------------------------------------------------
//
// streamsocket.cpp
//
//---------------------------------------------------------------------------

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "my_types.h"
#include "debug.h"
#include "streamsocket.h"


//---------------------------------------------------------------------------
//
// StreamSocket::StreamSocket(void)
//
//---------------------------------------------------------------------------
StreamSocket::StreamSocket(void)
{
 mSockId=0;
// dprintf("StreamSocket::StreamSocket - construction OK\n");
}


//---------------------------------------------------------------------------
//
// StreamSocket::~StreamSocket(void)
//
//---------------------------------------------------------------------------
StreamSocket::~StreamSocket(void)
{
 require(mSockId==0);
// dprintf("StreamSocket::~StreamSocket - destruction OK\n");
}


//---------------------------------------------------------------------------
//
// int StreamSocket::write(char* pInputBuffer,int bufferSize)
//
//---------------------------------------------------------------------------
int StreamSocket::write(const char *pInputBuffer, int bufferSize)
{
 require(mSockId!=0);
 return(::write(mSockId,pInputBuffer,bufferSize));
}


//---------------------------------------------------------------------------
//
// int StreamSocket::send(char* pInputBuffer,int bufferSize,int flags)
//
//---------------------------------------------------------------------------
int StreamSocket::send(const char *pInputBuffer, int bufferSize, int flags)
{
 require(mSockId!=0);
 return(::send(mSockId,pInputBuffer,bufferSize,flags));
}


//---------------------------------------------------------------------------
//
// int StreamSocket::read(char* pInputBuffer,int bufferSize)
//
//---------------------------------------------------------------------------
int StreamSocket::read(char* pInputBuffer,int bufferSize)
{
 require(mSockId!=0);
 return(::read(mSockId,pInputBuffer,bufferSize));
}


//---------------------------------------------------------------------------
//
// int StreamSocket::recv(char* pInputBuffer,int bufferSize,int flags)
//
//---------------------------------------------------------------------------
int StreamSocket::recv(char* pInputBuffer,int bufferSize,int flags)
{
 require(mSockId!=0);
 return(::recv(mSockId,pInputBuffer,bufferSize,flags));
}


//---------------------------------------------------------------------------
//
// int StreamSocket::enable_blocking(void)
//
//---------------------------------------------------------------------------
int StreamSocket::enable_blocking(void)
{
int block=0;

 require(mSockId!=0);
 //if(ioctl(mSockId,FIONBIO,(int)&block)<0)
 if(ioctl(mSockId,FIONBIO,&block)<0)		// AR - 64bit fix
    {
    printf("Socket::enable_blocking error - ioctl (%d)\n",errno);
    return(-1);
    }

// dprintf("Socket::enable_blocking - enabled blocking mode\n");

 return(0);
}


//---------------------------------------------------------------------------
//
// int StreamSocket::disable_blocking(void) 
//
//---------------------------------------------------------------------------
int StreamSocket::disable_blocking(void) 
{
int dontBlock=1;

 require(mSockId!=0);
 //if(ioctl(mSockId,FIONBIO,(int)&dontBlock)<0)
 if(ioctl(mSockId,FIONBIO,&dontBlock)<0)		// AR - 64bit fix
    {
    printf("StreamSocket::disable_blocking error - ioctl (%d)\n",errno);
    return(-1);
    }

// dprintf("StreamSocket::disable_blocking - disabled blocking mode\n");

 return(0);
}


//---------------------------------------------------------------------------
//
// int StreamSocket::enable_no_delay(void)
//
//---------------------------------------------------------------------------
int StreamSocket::enable_no_delay(void)
{
int on=1;       // activation flag for setsockopt

 require(mSockId!=0);
 if(setsockopt(mSockId,IPPROTO_TCP,TCP_NODELAY,(char *)&on,sizeof(on))<0)
    {
    printf("StreamSocket::enable_no_delay error - setsockopt (TCP_NODELAY option) (%d)\n",errno);
    return(-1);
    }

// dprintf("StreamSocket::enable_no_delay - enabled TCP_NODELAY option\n");

 return(0);
}


//---------------------------------------------------------------------------
//
// int StreamSocket::disable_no_delay(void)
//
//---------------------------------------------------------------------------
int StreamSocket::disable_no_delay(void)
{
int on=0;       // disactivation flag for setsockopt

 require(mSockId!=0);
 if(setsockopt(mSockId,IPPROTO_TCP,TCP_NODELAY,(char *)&on,sizeof(on))<0)
    {
    printf("StreamSocket::enable_no_delay error - setsockopt (TCP_NODELAY option) (%d)\n",errno);
    return(-1);
    }

// dprintf("StreamSocket::disable_no_delay - disabled TCP_NODELAY option\n");

 return(0);
}
