/*=========================================================================*\
||                                                                         ||
||                  Consiglio Nazionale delle Ricerche                     ||
||                                                                         ||
\*=========================================================================*/

// Release 1.1, 10/06/2004 Added some minor modifications 
// Release 1.0, 05/09/2003

//---------------------------------------------------------------------------
//
// sio.h
//
//---------------------------------------------------------------------------

#ifndef SIO_H
#define SIO_H

#include <termios.h>
#include <unistd.h>

typedef signed char  int8;
typedef signed short int16;
typedef signed long  int32;

typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned long  uint32;

typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned long  ulong;


class Sio
   {
   private:
      int mFdCom;
      struct termios mOldTio,mNewTio; 					  
 
   public:
      Sio(void);
      ~Sio(void);

      int init(const char* device    ="/dev/ttyS0",
               uint32 baudRate       =B9600, // 9600 baud
               uint32 dataBits       =CS8,   // 8 data bits
               uint32 stopBits       =0,     // one stop bit
               uint32 parityEnable   =0,     // parity disabled
               uint32 parityType     =0,     // parity even
               uint32 receivingEnable=0,     // receiver disabled
               uint32 noBlockingFlag =0      // blocking
               );
      int terminate(void);
      ssize_t read(void* buffer,size_t bufferSize){return(::read(mFdCom,buffer,bufferSize));};
      ssize_t write(const void* buffer,size_t bufferSize){return(::write(mFdCom,buffer,bufferSize));};
      int flush(){ if(tcflush(mFdCom,TCIOFLUSH)<0) {
      printf("Sio::init error - flush %s\n",strerror(errno));
      return(-1);
      }return 0;}
   };

#endif // SIO_H
