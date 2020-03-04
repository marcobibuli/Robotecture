/*=========================================================================*\
||                                                                         ||
||                  Code written by: Gabriele Bruzzone                     ||
||                                                                         ||
\*=========================================================================*/


// Release 2.0, 14/02/2020
// Release 1.0, 25/03/2016

//---------------------------------------------------------------------------
//
// e-URoPelib.cpp
//
//---------------------------------------------------------------------------

#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <sys/io.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

//#include "my_types.h"
#include "e-URoPelib.h"

#define DIAMOND_0_BASE_ADDRESS (0x300)
#define DIAMOND_1_BASE_ADDRESS (0x380)

#define DIAMOND_DIO_OUTPUT_PORT                (0)
#define DIAMOND_DIO_INPUT_PORT                 (1)
#define DIAMOND_IO_SPACE_SIZE                 (16)
#define DIAMOND_AD_MIN_VOLTAGE             (-10.0)
#define DIAMOND_AD_MAX_VOLTAGE             (+10.0)
#define DIAMOND_DA_MIN_VOLTAGE             (-10.0)
#define DIAMOND_DA_MAX_VOLTAGE             (+10.0)

#define DIAMOND_AD_RAW_SPAN              (65536.0)
#define DIAMOND_AD_VOLTAGE_SPAN (DIAMOND_AD_MAX_VOLTAGE-DIAMOND_AD_MIN_VOLTAGE)

#define DIAMOND_DA_RAW_SPAN              (65536.0)
#define DIAMOND_DA_VOLTAGE_SPAN (DIAMOND_DA_MAX_VOLTAGE-DIAMOND_DA_MIN_VOLTAGE)
#define DIAMOND_DA_MAX_UINT16            (65535.0)
#define DIAMOND_DA_MIN_UINT16                (0.0)
#define DIAMOND_DA_MAX_RAW_DATA           (0xFFFF)
#define DIAMOND_DA_MIN_RAW_DATA           (0x0000)

// all pages
#define DIAMOND_AD_START_OFFSET                (0)
#define DIAMOND_AD_LSB_OFFSET                  (0)
#define DIAMOND_AD_MSB_OFFSET                  (1)
#define DIAMOND_AD_LOW_CHANNEL_OFFSET          (2)
#define DIAMOND_AD_HIGH_CHANNEL_OFFSET         (3)
#define DIAMOND_DA_LSB12_OFFSET                (4)
#define DIAMOND_DA_MSB12_OFFSET                (5)
#define DIAMOND_AD_FIFO_DEPTH_OFFSET           (6)
#define DIAMOND_AD_FIFO_CONTROL_OFFSET         (7)
#define DIAMOND_MISC_AND_PAGE_CONTROL_OFFSET   (8)
#define DIAMOND_AD_STATUS_OFFSET               (8)
#define DIAMOND_COUNTER_TIMER_CONTROL_OFFSET  (10)
#define DIAMOND_AD_CONFIG_OFFSET              (11)

// page 0
#define DIAMOND_COUNTER_0_DATA_OFFSET         (12)
#define DIAMOND_COUNTERS_CONTROL_OFFSET       (15)

// page 2
#define DIAMOND_DIO_PORT_A_OFFSET             (12)
#define DIAMOND_DIO_PORT_B_OFFSET             (13)
#define DIAMOND_DIO_PORT_C_OFFSET             (14)
#define DIAMOND_DIO_CONTROL_OFFSET            (15)

// page 3 (enhanced feature)
#define DIAMOND_ENHANCED_FEATURES_OFFSET      (15)

// page 7 (enhanced feature)
#define DIAMOND_DA_LSB16_OFFSET               (12)
#define DIAMOND_DA_MSB16_OFFSET               (13)
#define DIAMOND_DA_CONFIG_OFFSET              (14)

#define DIAMOND_AD_TIME_OUT_US              (1000)
#define DIAMOND_DA_TIME_OUT_US              (1000)

#define CTD_304_PLUS_SERIAL_BAUD_RATE  (9600)
#define PA_500_SERIAL_BAUD_RATE        (9600)
#define IS3DMGX335_SERIAL_BAUD_RATE  (115200)

#define AHRS_DATA_SET                              (0x80)
#define GPS_DATA_SET                               (0x81)

#define SCALED_ACCELEROMETER_VECTOR_AHRS_DATA_DESCRIPTOR (0x04)
#define SCALED_GYRO_VECTOR_AHRS_DATA_DESCRIPTOR          (0x05)
#define SCALED_MAGNETOMETER_VECTOR_AHRS_DATA_DESCRIPTOR  (0x06)
#define ORIENTATION_MATRIX_AHRS_DATA_DESCRIPTOR          (0x09)
#define EULER_ANGLES_AHRS_DATA_DESCRIPTOR                (0x0C)

#define LLH_POSITION_GPS_DATA_DESCRIPTOR                 (0x03)
#define NED_VELOCITY_GPS_DATA_DESCRIPTOR                 (0x05)


//---------------------------------------------------------------------------
//
// int diamond_init(Diamond* pDiamond,ushort baseAddress,int portType[DIAMOND_DIO_MAX_PORTS],
//                  int negatedBit[DIAMOND_DIO_MAX_CHANNELS])
//
//---------------------------------------------------------------------------
int diamond_init(Diamond* pDiamond,ushort baseAddress,int portType[DIAMOND_DIO_MAX_PORTS],
                 int negatedBit[DIAMOND_DIO_MAX_CHANNELS])
{
 pDiamond->baseAddress=baseAddress;
 if(ioperm(baseAddress,DIAMOND_IO_SPACE_SIZE,1)<0)
    {
    pdebug("diamond_init error - ioperm %s\n",strerror(errno));
    return(-1);
    }

 pDiamond->rADStart=(ushort)(baseAddress+DIAMOND_AD_START_OFFSET);
 pDiamond->rADLSB=(ushort)(baseAddress+DIAMOND_AD_LSB_OFFSET);
 pDiamond->rADMSB=(ushort)(baseAddress+DIAMOND_AD_MSB_OFFSET);
 pDiamond->rADLowChannel=(ushort)(baseAddress+DIAMOND_AD_LOW_CHANNEL_OFFSET);
 pDiamond->rADHighChannel=(ushort)(baseAddress+DIAMOND_AD_HIGH_CHANNEL_OFFSET);
 pDiamond->rDALSB12=(ushort)(baseAddress+DIAMOND_DA_LSB12_OFFSET);
 pDiamond->rDAMSB12=(ushort)(baseAddress+DIAMOND_DA_MSB12_OFFSET);
 pDiamond->rADFIFODepth=(ushort)(baseAddress+DIAMOND_AD_FIFO_DEPTH_OFFSET);
 pDiamond->rADFIFOControl=(ushort)(baseAddress+DIAMOND_AD_FIFO_CONTROL_OFFSET);
 pDiamond->rMiscAndPageControl=(ushort)(baseAddress+DIAMOND_MISC_AND_PAGE_CONTROL_OFFSET);
 pDiamond->rADStatus=(ushort)(baseAddress+DIAMOND_AD_STATUS_OFFSET);
 pDiamond->rADConfig=(ushort)(baseAddress+DIAMOND_AD_CONFIG_OFFSET);
 pDiamond->rDIOPortA=(ushort)(baseAddress+DIAMOND_DIO_PORT_A_OFFSET);
 pDiamond->rDIOPortB=(ushort)(baseAddress+DIAMOND_DIO_PORT_B_OFFSET);
 pDiamond->rDIOPortC=(ushort)(baseAddress+DIAMOND_DIO_PORT_C_OFFSET);
 pDiamond->rDIOControl=(ushort)(baseAddress+DIAMOND_DIO_CONTROL_OFFSET);
 pDiamond->rEnhancedFeatures=(ushort)(baseAddress+DIAMOND_ENHANCED_FEATURES_OFFSET);
 pDiamond->rDALSB16=(ushort)(baseAddress+DIAMOND_DA_LSB16_OFFSET);
 pDiamond->rDAMSB16=(ushort)(baseAddress+DIAMOND_DA_MSB16_OFFSET);

 // select page 0 (default operating page)
 outb(0x00,pDiamond->rMiscAndPageControl);

 diamond_reset(pDiamond);

 outb(0,pDiamond->rADLowChannel);
// pdebug("diamond_init - A/D low channel: 0x%hX (0x%hX)\n",pDiamond->rADLowChannel,inb(pDiamond->rADLowChannel));

 outb(DIAMOND_AD_MAX_CHANNELS-1,pDiamond->rADHighChannel);
// pdebug("diamond_init - A/D high channel: 0x%hX (0x%hX)\n",pDiamond->rADHighChannel,inb(pDiamond->rADHighChannel));

 // set A/D gain to 1 and range to [-10;+10]
uint8_t byteValue;
 byteValue=inb(pDiamond->rADConfig);
 byteValue=(byteValue&0xF0)|0x08;
 outb(byteValue,pDiamond->rADConfig);
// pdebug("diamond_init - A/D config register: 0x%hX (0x%hX)\n",pDiamond->rADConfig,inb(pDiamond->rADConfig));

 // enable A/D scan
 outb(0x04,pDiamond->rADFIFOControl);
// pdebug("diamond_init - A/D FIFO control register: 0x%hX (0x%hX)\n",pDiamond->rADFIFOControl,inb(pDiamond->rADFIFOControl));

int i;
 for(i=0;i<DIAMOND_DIO_MAX_PORTS;i++)
    {
    if((portType[i]!=DIAMOND_DIO_OUTPUT_PORT)&&(portType[i]!=DIAMOND_DIO_INPUT_PORT))
       {
       pdebug("diamond_init - wrong DIO port type: %d %d\n",i,portType[i]);
       return(-1);
       }
    pDiamond->rDIOPortType[i]=portType[i];
    }

 // select page 1 to access to digital I/O settings
 outb(0x01,pDiamond->rMiscAndPageControl);

uint8_t writeByte=0x80; // bit 7 must be set to 1 and bit 2, 5 and 6 set to 0
 if(portType[0]==DIAMOND_DIO_INPUT_PORT)
    {
    writeByte|=0x10;
    }
 if(portType[1]==DIAMOND_DIO_INPUT_PORT)
    {
    writeByte|=0x02;
    }
 if(portType[2]==DIAMOND_DIO_INPUT_PORT)
    {
    writeByte|=0x01;
    }
 if(portType[3]==DIAMOND_DIO_INPUT_PORT)
    {
    writeByte|=0x08;
    }

// pdebug("diamond_init - DIO control: 0x%hX\n",(ushort)writeByte);
 outb(writeByte,pDiamond->rDIOControl);

 // select page 3 to enable enhanced features
 outb(0x03,pDiamond->rMiscAndPageControl);

 // enable enhanced features  (to have simultaneous D/A update and access to 16 bit D/A registers)
 outb(0xA6,pDiamond->rEnhancedFeatures);

 // select page 0 again (default operating page)
 outb(0x00,pDiamond->rMiscAndPageControl);

 if(negatedBit!=NULL)
    {
    for(i=0;i<DIAMOND_DIO_MAX_CHANNELS;i++)
       {
       pDiamond->negatedBit[i]=negatedBit[i];
       }
    }
 else
    {
    for(i=0;i<DIAMOND_DIO_MAX_CHANNELS;i++)
       {
       pDiamond->negatedBit[i]=0;
       }
    }
 pdebug("diamond_init - initialization OK at address : 0x%hX\n",baseAddress);
 return(0);
}


//---------------------------------------------------------------------------
//
// void diamond_reset(Diamond* pDiamond)
//
//---------------------------------------------------------------------------
void diamond_reset(Diamond* pDiamond)
{
 outb(0x20,pDiamond->rMiscAndPageControl);
}


//---------------------------------------------------------------------------
//
// int diamond_ad_get(Diamond* pDiamond,double voltage[DIAMOND_AD_MAX_CHANNELS])
//
//---------------------------------------------------------------------------
int diamond_ad_get(Diamond* pDiamond,double voltage[DIAMOND_AD_MAX_CHANNELS])
{

 outb(0x01,pDiamond->rADStart);

int timeOut=DIAMOND_AD_TIME_OUT_US;
struct timespec deadLine;
uint8_t statusByte;
 do{
   statusByte=inb(pDiamond->rADStatus)&0x80;
   deadLine.tv_sec=0;
   deadLine.tv_nsec=1000;
   clock_nanosleep(CLOCK_REALTIME,0,&deadLine,NULL);
   timeOut--;
   }while((statusByte!=0)&&(timeOut>0));
 if(timeOut==0)
    {
    pdebug("diamond_ad_get - Time out elapsed reading ADBUSY bit\n");
    return(-1);
    }

// pdebug("diamond FIFO depth: %d\n",inb(pDiamond->rADFIFODepth));

uint16_t lsb,msb;
int16_t rawInput;
int ch;
 for(ch=0;ch<DIAMOND_AD_MAX_CHANNELS;ch++)
    {
//    pdebug("FIFO depth: %d\n",inb(pDiamond->rADFIFODepth));
//    rawInput=inw(pDiamond->rADData);
    lsb=(ushort)inb(pDiamond->rADLSB);
    msb=(ushort)inb(pDiamond->rADMSB);
    rawInput=lsb|(msb<<8);
    voltage[ch]=(DIAMOND_AD_VOLTAGE_SPAN*rawInput)/DIAMOND_AD_RAW_SPAN;
//    pdebug("diamond_ad_get - raw input: (%d) 0x%hX %8.4f\n",ch,rawInput,voltage[ch]);
    }
// pdebug("diamond FIFO depth: %d\n",inb(pDiamond->rADFIFODepth));
 return(0);
}


//---------------------------------------------------------------------------
//
// int diamond_da_put(Diamond* pDiamond,double voltage[DIAMOND_DA_MAX_CHANNELS])
//
//---------------------------------------------------------------------------
int diamond_da_put(Diamond* pDiamond,double voltage[DIAMOND_DA_MAX_CHANNELS])
{
 // select page 7 to access to analog output settings
 outb(0x07,pDiamond->rMiscAndPageControl);
// pdebug("diamond DA config: 0x%02X\n",inb(pDiamond->rDAConfig));

//int i;
// for(i=0;i<DIAMOND_DA_MAX_CHANNELS;i++)
//    {
////    pdebug("voltage[%d]=%8.4f\n",i,voltage[i]);
//    printf("%8.4f ",voltage[i]);
//    }
// printf("\n");

double rawOutputVoltage;
uint32_t rawOutput;
uint8_t writeByte;
struct timespec deadLine;
int timeOut;
uint8_t statusByte;
int ch;

 for(ch=0;ch<DIAMOND_DA_MAX_CHANNELS;ch++)
    {
    rawOutputVoltage=round(((voltage[ch]-DIAMOND_DA_MIN_VOLTAGE)*DIAMOND_DA_RAW_SPAN)/DIAMOND_DA_VOLTAGE_SPAN);
    if(rawOutputVoltage>=DIAMOND_DA_MAX_UINT16)
       {
       rawOutput=DIAMOND_DA_MAX_RAW_DATA;
       }
    else
       {
       if(rawOutputVoltage<=DIAMOND_DA_MIN_UINT16)
          {
          rawOutput=DIAMOND_DA_MIN_RAW_DATA;
          }
       else
          {
          rawOutput=(uint16_t)rawOutputVoltage;
          }
       }
    outb(rawOutput&0x00FF,pDiamond->rDALSB16);
    outb((rawOutput&0xFF00)>>8,pDiamond->rDAMSB16);
    writeByte=(ch!=(DIAMOND_DA_MAX_CHANNELS-1))?(ch<<6)|0x20:(ch<<6); // update only last channel
//    pdebug("writeByte: 0x%02X\n",writeByte);
    outb(writeByte,pDiamond->rDAMSB12);

    timeOut=DIAMOND_DA_TIME_OUT_US;
    do{
      statusByte=inb(pDiamond->rDALSB12)&0x80;
    //      pdebug("%d\n",statusByte);
      deadLine.tv_sec=0;
      deadLine.tv_nsec=1000;
      clock_nanosleep(CLOCK_REALTIME,0,&deadLine,NULL);
      timeOut--;
      }while((statusByte!=0)&&(timeOut>0));
    //    pdebug("%d\n",statusByte);
    if(timeOut==0)
       {
       pdebug("diamond_da_put - Time out elapsed reading DACBUSY bit\n");
       return(-1);
       }
    }

 // select page 0 again (default operating page)
 outb(0x00,pDiamond->rMiscAndPageControl);

 return(0);
}


//---------------------------------------------------------------------------
//
// int diamond_di_get(Diamond* pDiamond,int bit[DIAMOND_DIO_MAX_CHANNELS])
//
//---------------------------------------------------------------------------
int diamond_di_get(Diamond* pDiamond,int bit[DIAMOND_DIO_MAX_CHANNELS])
{
 // select page 1 to access to digital I/O settings
 outb(0x01,pDiamond->rMiscAndPageControl);

uint8_t readByte=inb(pDiamond->rDIOPortA);
int offset=0;
int i;

 for(i=0;i<8;i++)
    {
    bit[offset]=readByte&0x01;
    if(pDiamond->negatedBit[offset]==1)
       {
       bit[offset]=!bit[offset];
       }
    offset++;
    readByte>>=1;
    }
 readByte=inb(pDiamond->rDIOPortB);
 for(i=0;i<8;i++)
    {
    bit[offset]=readByte&0x01;
    if(pDiamond->negatedBit[offset]==1)
       {
       bit[offset]=!bit[offset];
       }
    offset++;
    readByte>>=1;
    }
 readByte=inb(pDiamond->rDIOPortC);
 for(i=0;i<8;i++)
    {
    bit[offset]=readByte&0x01;
    if(pDiamond->negatedBit[offset]==1)
       {
       bit[offset]=!bit[offset];
       }
    offset++;
    readByte>>=1;
    }

 // select page 0 again (default operating page)
 outb(0x00,pDiamond->rMiscAndPageControl);
 return(0);
}


//---------------------------------------------------------------------------
//
// int diamond_do_put(Diamond* pDiamond,const int bit[DIAMOND_DIO_MAX_CHANNELS])
//
//---------------------------------------------------------------------------
int diamond_do_put(Diamond* pDiamond,const int bit[DIAMOND_DIO_MAX_CHANNELS])
{
 // select page 1 to access to digital I/O settings
 outb(0x01,pDiamond->rMiscAndPageControl);

uint8_t writeByte;
int offset=0;
int value;
int i;

 if(pDiamond->rDIOPortType[0]==DIAMOND_DIO_INPUT_PORT)
    {
    // do nothing
//       pdebug("DIO_INPUT_PORT\n");
    }
 else // DIO_OUTPUT_PORT
    {
    writeByte=0x00;
    for(i=0;i<8;i++)
       {
       value=bit[offset+i];
       if(pDiamond->negatedBit[offset+i]==1)
          {
          value=!value;
          }
       if((value!=0)&&(value!=1))
          {
          pdebug("diamond_dio_put error - wrong value: %d\n",value);
          return(-1);
          }
       writeByte|=(value<<i);
       }
    outb(writeByte,pDiamond->rDIOPortA);
//    pdebug("diamond DIOPortA: 0x%02X\n",writeByte);
    }

 offset+=8;
 if(pDiamond->rDIOPortType[1]==DIAMOND_DIO_INPUT_PORT)
    {
    // do nothing
//       pdebug("diamond DIO_INPUT_PORT\n");
    }
 else // DIO_OUTPUT_PORT
    {
    writeByte=0x00;
    for(i=0;i<8;i++)
       {
       value=bit[offset+i];
       if(pDiamond->negatedBit[offset+i]==1)
          {
          value=!value;
          }
       if((value!=0)&&(value!=1))
          {
          pdebug("diamond_dio_put error - wrong value: %d\n",value);
          return(-1);
          }
       writeByte|=(value<<i);
       }
    outb(writeByte,pDiamond->rDIOPortB);
//    pdebug("diamond DIOPortB: 0x%02X\n",writeByte);
    }

 offset+=8;
 if((pDiamond->rDIOPortType[2]==DIAMOND_DIO_INPUT_PORT)&&(pDiamond->rDIOPortType[3]==DIAMOND_DIO_INPUT_PORT))
    {
    // do nothing
//       pdebug("diamond DIO_INPUT_PORT\n");
    }
 else if((pDiamond->rDIOPortType[2]==DIAMOND_DIO_OUTPUT_PORT)&&(pDiamond->rDIOPortType[3]==DIAMOND_DIO_OUTPUT_PORT))
    {
    writeByte=0x00;
    for(i=0;i<8;i++)
       {
       value=bit[offset+i];
       if(pDiamond->negatedBit[offset+i]==1)
          {
          value=!value;
          }
       if((value!=0)&&(value!=1))
          {
          pdebug("diamond_dio_put error - wrong value: %d\n",value);
          return(-1);
          }
       writeByte|=(value<<i);
       }
    outb(writeByte,pDiamond->rDIOPortC);
//    pdebug("diamond DIOPortC: 0x%02X\n",writeByte);
    }
 else if((pDiamond->rDIOPortType[2]==DIAMOND_DIO_OUTPUT_PORT)&&(pDiamond->rDIOPortType[3]==DIAMOND_DIO_INPUT_PORT))
    {
    writeByte=inb(pDiamond->rDIOPortC)&0xF0;
    for(i=0;i<4;i++)
       {
       value=bit[offset+i];
       if(pDiamond->negatedBit[offset+i]==1)
          {
          value=!value;
          }
       if((value!=0)&&(value!=1))
          {
          pdebug("diamond_dio_put error - wrong value: %d\n",value);
          return(-1);
          }
       writeByte|=(value<<i);
       }
    outb(writeByte,pDiamond->rDIOPortC);
//    pdebug("diamond mDIOPortC: 0x%02X\n",writeByte);
    }
 else // if((pDiamond->rDIOPortType[2]==DIAMOND_DIO_INPUT_PORT)&&(pDiamond->rDIOPortType[3]==DIAMOND_DIO_OUTPUT_PORT))
    {
    writeByte=inb(pDiamond->rDIOPortC)&0x0F;
    for(i=4;i<8;i++)
       {
       value=bit[offset+i];
       if(pDiamond->negatedBit[offset+i]==1)
          {
          value=!value;
          }
       if((value!=0)&&(value!=1))
          {
          pdebug("diamond_dio_put error - wrong value: %d\n",value);
          return(-1);
          }
       writeByte|=(value<<i);
       }
    outb(writeByte,pDiamond->rDIOPortC);
//    pdebug("diamond DIOPortC: 0x%02X\n",writeByte);
    }

 // select page 0 again (default operating page)
 outb(0x00,pDiamond->rMiscAndPageControl);
 return(0);
}


//---------------------------------------------------------------------------
//
// int diamond_dio_is_input(Diamond* pDiamond,int ch)
//
//---------------------------------------------------------------------------
int diamond_dio_is_input(Diamond* pDiamond,int ch)
{
 if((ch>=0)&&(ch<=7))
    {
    return(pDiamond->rDIOPortType[0]==DIAMOND_DIO_INPUT_PORT?1:0);
    }
 else if((ch>=8)&&(ch<=15))
    {
    return(pDiamond->rDIOPortType[1]==DIAMOND_DIO_INPUT_PORT?1:0);
    }
 else if((ch>=16)&&(ch<=19))
    {
    return(pDiamond->rDIOPortType[2]==DIAMOND_DIO_INPUT_PORT?1:0);
    }
 else if((ch>=20)&&(ch<=23))
    {
    return(pDiamond->rDIOPortType[3]==DIAMOND_DIO_INPUT_PORT?1:0);
    }
 else
    {
    pdebug("diamond_dio_get_channel_type - bad channel %d\n",ch);
    return(-1);
    }
}


//---------------------------------------------------------------------------
//
// int diamond_terminate(Diamond* pDiamond)
//
//---------------------------------------------------------------------------
int diamond_terminate(Diamond* pDiamond)
{
 // select page 3 to disable enhanced features
 outb(0x03,pDiamond->rMiscAndPageControl);
 // disable enhanced features
 outb(0xA7,pDiamond->rEnhancedFeatures);
 // select page 0 again (default operating page)
 outb(0x00,pDiamond->rMiscAndPageControl);
 if(ioperm(pDiamond->baseAddress,DIAMOND_IO_SPACE_SIZE,0)<0)
    {
    pdebug("diamond_terminate error - ioperm %s\n",strerror(errno));
    return(-1);
    }

 pdebug("diamond_terminate - termination OK at address 0x%X\n",(int)pDiamond->baseAddress);
 return(0);
}


//---------------------------------------------------------------------------
//
// int serial_init(const char* device,int baudRate)
//
//---------------------------------------------------------------------------
int serial_init(const char* device,int baudRate)
{
int fd=-1;
 switch(baudRate)
    {
    case 50:
       baudRate=B50;
    break;
    case 75:
       baudRate=B75;
    break;
    case 110:
       baudRate=B110;
    break;
    case 134:
       baudRate=B134;
    break;
    case 200:
       baudRate=B200;
    break;
    case 300:
       baudRate=B300;
    break;
    case 600:
       baudRate=B600;
    break;
    case 1200:
       baudRate=B1200;
    break;
    case 1800:
       baudRate=B1800;
    break;
    case 2400:
       baudRate=B2400;
    break;
    case 4800:
       baudRate=B4800;
    break;
    case 9600:
       baudRate=B9600;
    break;
    case 19200:
       baudRate=B19200;
    break;
    case 38400:
       baudRate=B38400;
    break;
    case 57600:
       baudRate=B57600;
    break;
    case 115200:
       baudRate=B115200;
    break;
    case 230400:
       baudRate=B230400;
    break;
    default:
       pdebug("serial_init error - wrong baud rate: %u\n",baudRate);
       return(-1);
    break;
    }
 
 if((fd=open(device,O_RDWR|O_NONBLOCK))<0)
    {
    pdebug("serial_init error - open %s\n",strerror(errno));
    return(-1);
    }
 
 if(tcflush(fd,TCIOFLUSH)<0)
    {
    pdebug("serial_init error - flush %s\n",strerror(errno));
    return(-1);
    }

struct termios tio;
 memset(&tio,0,sizeof(tio));
 tio.c_iflag=IGNBRK|IGNPAR;
 tio.c_cflag=CS8|CREAD|CLOCAL;
 tio.c_cc[VMIN] =1;
 tio.c_cc[VTIME]=0;
 
 if(cfsetospeed(&tio,baudRate)<0)
    {
    pdebug("serial_init error - cfsetospeed %s\n",strerror(errno));
    return(-1);
    }
 
 if(cfsetispeed(&tio,baudRate)<0)
    {
    pdebug("serial_init error - cfsetospeed %s\n",strerror(errno));
    return(-1);
    }
 
 tcsetattr(fd,TCSANOW,&tio);

 
 return(fd);
}


//---------------------------------------------------------------------------
//
// int serial_terminate(int fd)
//
//---------------------------------------------------------------------------
int serial_terminate(int fd)
{
 if(close(fd)<0)
    {
    pdebug("serial_terminate error - close %s\n",strerror(errno));
    return(-1);
    }
 return(0);
}


//---------------------------------------------------------------------------
//
// int ctd_304_plus_init(Ctd304Plus* pCtd304Plus,const char* device)
//
//---------------------------------------------------------------------------
int ctd_304_plus_init(Ctd304Plus* pCtd304Plus,const char* device)
{
 return(pCtd304Plus->fdSio=serial_init(device,CTD_304_PLUS_SERIAL_BAUD_RATE));
}


//---------------------------------------------------------------------------
//
// int ctd_304_plus_terminate(Ctd304Plus* pCtd304Plus)
//
//---------------------------------------------------------------------------
int ctd_304_plus_terminate(Ctd304Plus* pCtd304Plus)
{
 return(pCtd304Plus->fdSio=serial_terminate(pCtd304Plus->fdSio));
}

#define CTD_304_BUFFER_LENGTH (64)
//---------------------------------------------------------------------------
//
// static int ctd_304_plus_extract_line(char inChar,char* line,int lineMaxLength)
//
//---------------------------------------------------------------------------
static int ctd_304_plus_extract_line(char inChar,char* line,int lineMaxLength)
{
static int index=0;
static char buffer[CTD_304_BUFFER_LENGTH];

 if(index==(CTD_304_BUFFER_LENGTH-2)) // reject line (too long)
    {
    index=0;
    }
 buffer[index++]=inChar;
 if(inChar=='\n') // line terminated
    {
    buffer[index++]='\0';
    strncpy(line,buffer,lineMaxLength);
    index=0;
    return(0);
    }
 else
    {
    return(-1);
    }
}


//---------------------------------------------------------------------------
//
// int ctd_304_plus_read(Ctd304Plus* pCtd304Plus,double* pConductivity,
//                       double* pTemperature,double* pDepth)
//
//---------------------------------------------------------------------------
int ctd_304_plus_read(Ctd304Plus* pCtd304Plus,double* pConductivity,
                      double* pTemperature,double* pDepth)
{
int nRead;
char inChar;
char line[CTD_304_BUFFER_LENGTH];

 while(1)
    {
    nRead=read(pCtd304Plus->fdSio,&inChar,1);
//printf("%c",inChar);fflush(stdout);
    if(nRead==1)
       {
       if(ctd_304_plus_extract_line(inChar,line,sizeof(line))==0)
          {
          double tmpConductivity,tmpTemperature,tmpDepth;
          int err;
          if((err=sscanf(line,"%lf ,%lf ,%lf",&tmpConductivity,&tmpTemperature,&tmpDepth))==3)
             {
             *pConductivity=tmpConductivity;
             *pTemperature=tmpTemperature;
             *pDepth=tmpDepth;
             return(0);
             }
          else
             {
             pdebug("Wrong line: ===%s=== (%d)\n",line,err);
             return(-1);
             }
          }
       }
    else if(nRead<0)
       {
       if(errno==EWOULDBLOCK)
          {
          // do nothing
          }
       else
          {
          pdebug("Read error\n");
          }
       return(-1);
       }
    else
       {
       pdebug("Unexpected return value\n");
       return(-1);
       }
    }
 return(0);
}
#undef CTD_304_BUFFER_LENGTH


//---------------------------------------------------------------------------
//
// int write_with_error_check(int fd,const char* buffer,int bufferSize)
//
//---------------------------------------------------------------------------
int write_with_error_check(int fd,const char* buffer,int bufferSize)
{
//int i=0;
// for(i=0;i<bufferSize;i++)
//    {
//    printf("%c",buffer[i]);
//    }
// printf("@\n");
// fflush(stdout);

 if(write(fd,buffer,bufferSize)!=bufferSize)
    {
    pdebug("Error writing : %s\n",strerror(errno));
    return(-1);
    }
 return(0);
}


//---------------------------------------------------------------------------
//
// int pa500_init(PA500* pPA500,const char* device,int mode)
//
//---------------------------------------------------------------------------
int pa500_init(PA500* pPA500,const char* device,int mode)
{
//pdebug("%s %d\n",device,PA_500_SERIAL_BAUD_RATE);
 if((pPA500->fdSio=serial_init(device,PA_500_SERIAL_BAUD_RATE))<0)
    {
    pdebug("Error opening PA500 serial device: %s\n",device);
    return(-1);
    }
//pdebug("fdSio: %d\n",pPA500->fdSio);
  switch(mode)
    {
    case FREE_RUNNING_PA500_MODE:
       pPA500->mode=mode;
       write_with_error_check(pPA500->fdSio,"F",1);
    break;
    case INTERROGATE_PA500_MODE:
       pPA500->mode=mode;
       write_with_error_check(pPA500->fdSio,"Z",1);
    break;
    default:
       pdebug("Wrong PA500 mode: %d\n",mode);
       return(-1);
    break;
    }
 return(0);
}


//---------------------------------------------------------------------------
//
// int pa500_terminate(PA500* pPA500)
//
//---------------------------------------------------------------------------
int pa500_terminate(PA500* pPA500)
{
 write_with_error_check(pPA500->fdSio,"Z",1);
 return(pPA500->fdSio=serial_terminate(pPA500->fdSio));
}

#define PA500_BUFFER_LENGTH (64)
//---------------------------------------------------------------------------
//
// static int pa500_extract_line(char inChar,char* line,int lineMaxLength)
//
//---------------------------------------------------------------------------
static int pa500_extract_line(char inChar,char* line,int lineMaxLength)
{
static int index=0;
static char buffer[PA500_BUFFER_LENGTH];

 if(index==(PA500_BUFFER_LENGTH-2)) // reject line (too long)
    {
    index=0;
    }
 buffer[index++]=inChar;
 if(inChar=='\n') // line terminated
    {
    buffer[index++]='\0';
    strncpy(line,buffer,lineMaxLength);
    index=0;
    return(0);
    }
 else
    {
    return(-1);
    }
}


//---------------------------------------------------------------------------
//
// int pa500_read(PA500* pPA500,double* pRange)
//
//---------------------------------------------------------------------------
int pa500_read(PA500* pPA500,double* pRange)
{
int nRead;
char inChar;
char line[PA500_BUFFER_LENGTH];

 while(1)
    {
    nRead=read(pPA500->fdSio,&inChar,1);
    if(nRead==1)
       {
//printf("C: %d  0x%02hX - %c",pPA500->fdSio,(int)(unsigned char)inChar,inChar);fflush(stdout);
       if(pa500_extract_line(inChar,line,sizeof(line))==0)
          {
          double tmpRange;
          int err;
          if((err=sscanf(line,"%lfm",&tmpRange))==1)
             {
//             pdebug("Range: %6.2f\n",tmpRange);
             *pRange=tmpRange;
             return(0);
             }
          else
             {
             pdebug("Wrong line: ===%s=== (%d)\n",line,err);
             return(-1);
             }
          }
       }
    else if(nRead<0)
       {
       if(errno==EWOULDBLOCK)
          {
          // do nothing
          }
       else
          {
          pdebug("Read error\n");
          }
       return(-1);
       }
    else
       {
       pdebug("Unexpected return value\n");
       return(-1);
       }
    }
}
#undef PA500_BUFFER_LENGTH


//---------------------------------------------------------------------------
//
// int is3dmgx335_init(IS3DMGX335* pIS3DMGX335,const char* device)
//
//---------------------------------------------------------------------------
int is3dmgx335_init(IS3DMGX335* pIS3DMGX335,const char* device)
{
 return(pIS3DMGX335->fdSio=serial_init(device,IS3DMGX335_SERIAL_BAUD_RATE));
}


//---------------------------------------------------------------------------
//
// int is3dmgx335_terminate(IS3DMGX335* pIS3DMGX335)
//
//---------------------------------------------------------------------------
int is3dmgx335_terminate(IS3DMGX335* pIS3DMGX335)
{
 return(serial_terminate(pIS3DMGX335->fdSio));
}


#define extract_big_endian_float32_from_buffer(buffer,index,tmpUint32,result)\
 tmpUint32=*(uint32_t*)&buffer[index];\
 tmpUint32=ntohl(tmpUint32);\
 result=*(float*)&tmpUint32

#define extract_big_endian_double64_from_buffer(buffer,index,tmpUint64,result)\
 tmpUint64=*(uint64_t*)&buffer[index];\
 tmpUint64=endian_swap64(tmpUint64);\
 result=*(double*)&tmpUint64

#define extract_big_endian_uint32_from_buffer(buffer,index,tmpUint32,result)\
 tmpUint32=*(uint32_t*)&buffer[index];\
 result=ntohl(tmpUint32)

#define extract_big_endian_uint16_from_buffer(buffer,index,tmpUint16,result)\
 tmpUint16=*(uint16_t*)&buffer[index];\
 result=ntohs(tmpUint16)


//----------------------------------------------------------------------------
//
// static uint16_t compute_fletcher_checksum(const char *buffer,int bufferLength)
//
//----------------------------------------------------------------------------
static uint16_t compute_fletcher_checksum(const char *buffer,int bufferLength)
{
uint8_t msb=0;
uint8_t lsb=0;
int i;
 for(i=0;i<bufferLength;i++)
    {
    msb+=buffer[i];
    lsb+=msb;
    }
 return((((uint16_t)(msb))<<8)|(((uint16_t)(lsb))));
}


//---------------------------------------------------------------------------
//
// static int is3dmgx335_extract_body(char inChar,char* body)
//
//---------------------------------------------------------------------------
static int is3dmgx335_extract_body(char inChar,char* body)
{
static int index=0;
static int toRead=0;
static int bodyLength=0;

 if(index==0) // header first byte
    {
    if(inChar!='u')
       {
       return(-1);
       }
    else // the first byte is 'u': OK!
       {
       body[index++]=inChar;
       return(-1);
       }
    }
 else if(index==1) // header second byte
    {
    if(inChar!='e')
       {
       index=0;
       return(-1);
       }
    else // the second byte is 'e': OK!
       {
       body[index++]=inChar;
       return(-1);
       }
    }
 else if(index==2) // descriptor set
    {
    body[index++]=inChar;
    return(-1);
    }
 else if(index==3) // length
    {
    body[index++]=inChar;
    bodyLength=(int)((unsigned char)inChar);
    toRead=bodyLength+2; // adds 2 for reading also the Fletcher checksum field bytes
    return(-1);
    }
 else // body and Flecther checksum
    {
    body[index++]=inChar;
    toRead--;
    if(toRead==0) // check the CRC16
       {
       int messageFletcherChecksum=(body[index-1]&0x000000FF)|((body[index-2]<<8)&0x0000FF00);
       index=0;
       int computedFletcherChecksum=compute_fletcher_checksum(body,bodyLength+2+1+1); // adds 2 for headers field, 1 for descriptor set byte and 1 for payload length field byte
//       pdebug("0x%02hX 0x%02hX\n",messageFletcherChecksum,computedFletcherChecksum);
       if(messageFletcherChecksum==computedFletcherChecksum)
          {
          int i;
          for(i=0;i<bodyLength+2;i++) // adds 2 (1 for descriptor set byte and 1 for payload length field byte)
             {
             body[i]=body[i+2];
             }
          return(0);
          }
       else
          {
          pdebug("Bad Fletcher checksum\n");
          return(-1);
          }
       }
    return(-1);
    }
}


//---------------------------------------------------------------------------
//
// static int is3dmgx335_receive_message(IS3DMGX335* pIS3DMGX335,char* message)
//
//---------------------------------------------------------------------------
static int is3dmgx335_receive_message(IS3DMGX335* pIS3DMGX335,char* message)
{
int nRead;
char inChar;

 while(1)
    {
    nRead=read(pIS3DMGX335->fdSio,&inChar,1);
//printf("%c",inChar);fflush(stdout);
    if(nRead==1)
       {
       if(is3dmgx335_extract_body(inChar,message)==0)
          {
          return(0);
          }
       }
    else if(nRead<0)
       {
       if(errno==EWOULDBLOCK)
          {
          // do nothing
          }
       else
          {
          pdebug("Read error\n");
          }
       return(-1);
       }
    else
       {
       pdebug("Unexpected return value\n");
       return(-1);
       }
    }
}


//---------------------------------------------------------------------------
//
// static int is3dmgx335_get_field(const char* message,char* field,int* pDescriptorSet,
//                                 int* pFieldDescriptor,int* pIsFirstTime)
//
//---------------------------------------------------------------------------
static int is3dmgx335_get_field(const char* message,char* field,int* pDescriptorSet,
                                int* pFieldDescriptor,int* pIsFirstTime)
{
static int payloadLength;
static char* currentField;

 if(*pIsFirstTime)
    {
    payloadLength=(int)(uint8_t)message[1];
    currentField=(char*)&message[2];
    *pIsFirstTime=0;
    }
 if(payloadLength<=0)
    {
    return(-1);
    }
 *pDescriptorSet=(int)(uint8_t)message[0];
 *pFieldDescriptor=(int)(uint8_t)currentField[1];
int currentFieldLength=(int)(uint8_t)currentField[0];
 memcpy(field,currentField,currentFieldLength);
 currentField+=currentFieldLength;
 payloadLength-=currentFieldLength;
 return(0);
}


//---------------------------------------------------------------------------
//
// static void is3dmgx335_extract_scaled_accelerometer_data(const char*field,
//    double* pXAccel,double *pYAccel,double* pZAccel)
//
//---------------------------------------------------------------------------
static void is3dmgx335_extract_scaled_accelerometer_data(const char*field,
   double* pXAccel,double *pYAccel,double* pZAccel)
{
float xAccel,yAccel,zAccel;
uint32_t tmpUint32;
  extract_big_endian_float32_from_buffer(field,2+0,tmpUint32,xAccel);
  extract_big_endian_float32_from_buffer(field,2+4,tmpUint32,yAccel);
  extract_big_endian_float32_from_buffer(field,2+8,tmpUint32,zAccel);
 *pXAccel=xAccel;
 *pYAccel=yAccel;
 *pZAccel=zAccel;
}


//---------------------------------------------------------------------------
//
// static void is3dmgx335_extract_orientation_matrix_data(const char*field,
//    double orientationMatrix[3][3])
//
//---------------------------------------------------------------------------
static void is3dmgx335_extract_orientation_matrix_data(const char*field,
   double orientationMatrix[3][3])
{
float om[3][3];
uint32_t tmpUint32;
  extract_big_endian_float32_from_buffer(field,2+0,tmpUint32,om[0][0]);
  extract_big_endian_float32_from_buffer(field,2+4,tmpUint32,om[0][1]);
  extract_big_endian_float32_from_buffer(field,2+8,tmpUint32,om[0][2]);
  extract_big_endian_float32_from_buffer(field,2+12,tmpUint32,om[1][0]);
  extract_big_endian_float32_from_buffer(field,2+16,tmpUint32,om[1][1]);
  extract_big_endian_float32_from_buffer(field,2+20,tmpUint32,om[1][2]);
  extract_big_endian_float32_from_buffer(field,2+24,tmpUint32,om[2][0]);
  extract_big_endian_float32_from_buffer(field,2+28,tmpUint32,om[2][1]);
  extract_big_endian_float32_from_buffer(field,2+32,tmpUint32,om[2][2]);

int i,j;
  for(i=0;i<3;i++)
     {
     for(j=0;j<3;j++)
        {
        orientationMatrix[i][j]=om[i][j];
        }
     }
}


//---------------------------------------------------------------------------
//
// static void is3dmgx335_extract_scaled_gyro_data(const char*field,double* pXGyro,
//                                                 double *pYGyro,double* pZGyro)
//
//---------------------------------------------------------------------------
static void is3dmgx335_extract_scaled_gyro_data(const char*field,double* pXGyro,
                                                double *pYGyro,double* pZGyro)
{
float xGyro,yGyro,zGyro;
uint32_t tmpUint32;
  extract_big_endian_float32_from_buffer(field,2+0,tmpUint32,xGyro);
  extract_big_endian_float32_from_buffer(field,2+4,tmpUint32,yGyro);
  extract_big_endian_float32_from_buffer(field,2+8,tmpUint32,zGyro);
 *pXGyro=xGyro;
 *pYGyro=yGyro;
 *pZGyro=zGyro;
}


//---------------------------------------------------------------------------
//
// static void is3dmgx335_extract_scaled_magnetometer_data(const char*field,
//    double* pXMag,double *pYMag,double* pZMag)
//
//---------------------------------------------------------------------------
static void is3dmgx335_extract_scaled_magnetometer_data(const char*field,
   double* pXMag,double *pYMag,double* pZMag)
{
float xMag,yMag,zMag;
uint32_t tmpUint32;
  extract_big_endian_float32_from_buffer(field,2+0,tmpUint32,xMag);
  extract_big_endian_float32_from_buffer(field,2+4,tmpUint32,yMag);
  extract_big_endian_float32_from_buffer(field,2+8,tmpUint32,zMag);
 *pXMag=xMag;
 *pYMag=yMag;
 *pZMag=zMag;
}


//---------------------------------------------------------------------------
//
// static void is3dmgx335_extract_euler_angles(const char*field,double* pYaw,
//                                             double *pPitch,double* pRoll)
//
//---------------------------------------------------------------------------
static void is3dmgx335_extract_euler_angles(const char*field,double* pYaw,
                                            double *pPitch,double* pRoll)
{
float yaw,pitch,roll;
uint32_t tmpUint32;
 extract_big_endian_float32_from_buffer(field,2+0,tmpUint32,roll);
 extract_big_endian_float32_from_buffer(field,2+4,tmpUint32,pitch);
 extract_big_endian_float32_from_buffer(field,2+8,tmpUint32,yaw);
 *pYaw=yaw;
 *pPitch=pitch;
 *pRoll=roll;
}


//----------------------------------------------------------------------------
//
// static uint64_t endian_swap64(uint64_t value)
//
//----------------------------------------------------------------------------
static uint64_t endian_swap64(uint64_t value)
{
return((value>>56)&0x00000000000000FF)|
      ((value>>40)&0x000000000000FF00)|
      ((value>>24)&0x0000000000FF0000)|
      ((value>> 8)&0x00000000FF000000)|
      ((value<< 8)&0x000000FF00000000ull)|
      ((value<<24)&0x0000FF0000000000ull)|
      ((value<<40)&0x00FF000000000000ull)|
      ((value<<56)&0xFF00000000000000ull);
}


//---------------------------------------------------------------------------
//
// static void is3dmgx335_extract_llh_position(const char* field,double* pLatitude,double* pLongitude,
//                                             double* pHeightAboveEllipsode,double* pHeightAboveMSL,
//                                             double* pHorizontalAccuracy,double *pVerticalAccuracy,
//                                             uint16_t* pValidFlags)
//
//---------------------------------------------------------------------------
static void is3dmgx335_extract_llh_position(const char* field,double* pLatitude,double* pLongitude,
                                            double* pHeightAboveEllipsode,double* pHeightAboveMSL,
                                            double* pHorizontalAccuracy,double *pVerticalAccuracy,
                                            uint16_t* pValidFlags)
{
uint16_t tmpUint16;
uint32_t tmpUint32;
uint64_t tmpUint64;
double latitude,longitude,heightAboveEllipsode,heightAboveMSL;
float horizontalAccuracy,verticalAccuracy;
uint16_t validFlags;
 extract_big_endian_double64_from_buffer(field,2+0,tmpUint64,latitude);
 extract_big_endian_double64_from_buffer(field,2+8,tmpUint64,longitude);
 extract_big_endian_double64_from_buffer(field,2+16,tmpUint64,heightAboveEllipsode);
 extract_big_endian_double64_from_buffer(field,2+24,tmpUint64,heightAboveMSL);
 extract_big_endian_float32_from_buffer(field,2+32,tmpUint32,horizontalAccuracy);
 extract_big_endian_float32_from_buffer(field,2+36,tmpUint32,verticalAccuracy);
 extract_big_endian_uint16_from_buffer(field,2+40,tmpUint16,validFlags);
 *pLatitude=latitude;
 *pLongitude=longitude;
 *pHeightAboveEllipsode=heightAboveEllipsode;
 *pHeightAboveMSL=heightAboveMSL;
 *pHorizontalAccuracy=horizontalAccuracy;
 *pVerticalAccuracy=verticalAccuracy;
 *pValidFlags=validFlags;
}


//---------------------------------------------------------------------------
//
// static void is3dmgx335_extract_ned_velocity(const char* field,double* pNorth,double* pEast,
//                                             double* pDown,double* pSpeed,double* pGroundSpeed,
//                                             double* pHeading,double *pSpeedAccuracy,
//                                             double *pHeadingAccuracy,uint16_t* pValidFlags)
//
//---------------------------------------------------------------------------
static void is3dmgx335_extract_ned_velocity(const char* field,double* pNorth,double* pEast,
                                            double* pDown,double* pSpeed,double* pGroundSpeed,
                                            double* pHeading,double *pSpeedAccuracy,
                                            double *pHeadingAccuracy,uint16_t* pValidFlags)
{
uint16_t tmpUint16;
uint32_t tmpUint32;
float north,east,down,speed,groundSpeed,heading,speedAccuracy,headingAccuracy;
uint16_t validFlags;
 extract_big_endian_float32_from_buffer(field,2+0,tmpUint32,north);
 extract_big_endian_float32_from_buffer(field,2+4,tmpUint32,east);
 extract_big_endian_float32_from_buffer(field,2+8,tmpUint32,down);
 extract_big_endian_float32_from_buffer(field,2+12,tmpUint32,speed);
 extract_big_endian_float32_from_buffer(field,2+16,tmpUint32,groundSpeed);
 extract_big_endian_float32_from_buffer(field,2+20,tmpUint32,heading);
 extract_big_endian_float32_from_buffer(field,2+24,tmpUint32,speedAccuracy);
 extract_big_endian_float32_from_buffer(field,2+28,tmpUint32,headingAccuracy);
 extract_big_endian_uint16_from_buffer(field,2+32,tmpUint16,validFlags);
 *pNorth=north;
 *pEast=east;
 *pDown=down;
 *pSpeed=speed;
 *pGroundSpeed=groundSpeed;
 *pHeading=heading;
 *pSpeedAccuracy=speedAccuracy;
 *pHeadingAccuracy=headingAccuracy;
 *pValidFlags=validFlags;
}


//---------------------------------------------------------------------------
//
// int is3dmgx335_read(IS3DMGX335* pIS3DMGX335,IS3DMGX335Data* pData)
//
//---------------------------------------------------------------------------
int is3dmgx335_read(IS3DMGX335* pIS3DMGX335,IS3DMGX335Data* pData)
{
static char message[1024];

 while(is3dmgx335_receive_message(pIS3DMGX335,message)==0)
    {
    char field[1024];
    int isFirstTime=1;
    int descriptorSet;
    int fieldDescriptor;
    while(is3dmgx335_get_field(message,field,&descriptorSet,&fieldDescriptor,&isFirstTime)==0)
       {
       if((descriptorSet==AHRS_DATA_SET)&&(fieldDescriptor==SCALED_ACCELEROMETER_VECTOR_AHRS_DATA_DESCRIPTOR))
          {
          is3dmgx335_extract_scaled_accelerometer_data (field,&(pData->xAccel),&(pData->yAccel),&(pData->zAccel));
          pData->xAccel=pData->xAccel*9.80665;
          pData->yAccel=pData->yAccel*9.80665;
          pData->zAccel=pData->zAccel*9.80665;
//             pdebug("xA: %6.2f - yA: %6.2f - zA: %6.2f\n",pData->xAccel,pData->yAccel,pData->zAccel);
          }
       else if((descriptorSet==AHRS_DATA_SET)&&(fieldDescriptor==SCALED_GYRO_VECTOR_AHRS_DATA_DESCRIPTOR))
          {
          is3dmgx335_extract_scaled_gyro_data (field,&(pData->xGyro),&(pData->yGyro),&(pData->zGyro));
          pData->xGyro=-(pData->xGyro*180.0)/M_PI;
          pData->yGyro=-(pData->yGyro*180.0)/M_PI;
          pData->zGyro=(pData->zGyro*180.0)/M_PI;
//             pdebug("xG: %6.2f - yG: %6.2f - zG: %6.2f\n",pData->xGyro,pData->yGyro,pData->zGyro);
          }
       else if((descriptorSet==AHRS_DATA_SET)&&(fieldDescriptor==SCALED_MAGNETOMETER_VECTOR_AHRS_DATA_DESCRIPTOR))
          {
          is3dmgx335_extract_scaled_magnetometer_data (field,&(pData->xMag),&(pData->yMag),&(pData->zMag));
//             pdebug("xM: %6.2f - yM: %6.2f - zM: %6.2f\n",pData->xMag,pData->yMag,pData->zMag);
          }
       else if((descriptorSet==AHRS_DATA_SET)&&(fieldDescriptor==ORIENTATION_MATRIX_AHRS_DATA_DESCRIPTOR))
          {
          is3dmgx335_extract_orientation_matrix_data (field,pData->orientationMatrix);
//          pdebug("m11: %6.2f - m12: %6.2f - m13: %6.2f\n",pData->orientationMatrix[0][0],pData->orientationMatrix[0][1],pData->orientationMatrix[0][2]);
//          pdebug("m21: %6.2f - m22: %6.2f - m23: %6.2f\n",pData->orientationMatrix[1][0],pData->orientationMatrix[1][1],pData->orientationMatrix[1][2]);
//          pdebug("m31: %6.2f - m32: %6.2f - m33: %6.2f\n",pData->orientationMatrix[2][0],pData->orientationMatrix[2][1],pData->orientationMatrix[2][2]);
          }
       else if((descriptorSet==AHRS_DATA_SET)&&(fieldDescriptor==EULER_ANGLES_AHRS_DATA_DESCRIPTOR))
          {
          is3dmgx335_extract_euler_angles(field,&(pData->yaw),&(pData->pitch),&(pData->roll));
          pData->yaw=module_180(180.0+(pData->yaw*180.0)/M_PI);
          pData->pitch=-module_180((pData->pitch*180.0)/M_PI);
          pData->roll=-module_180((pData->roll*180.0)/M_PI);
//             pdebug("yaw: %6.2f - pitch: %6.2f - roll: %6.2f\n",pData->yaw,pData->pitch,pData->roll);
          }
       else if((descriptorSet==GPS_DATA_SET)&&(fieldDescriptor==LLH_POSITION_GPS_DATA_DESCRIPTOR))
          {
          is3dmgx335_extract_llh_position(field,&(pData->latitude),&(pData->longitude),&(pData->heightAboveEllipsode),
                                          &(pData->heightAboveMSL),&(pData->horizontalAccuracy),
                                          &(pData->verticalAccuracy),&(pData->llhPositionValidFlags));
//             pdebug("latitude: %6.2f - longitude: %6.2f\n",pData->latitude,pData->longitude);
          }
       else if((descriptorSet==GPS_DATA_SET)&&(fieldDescriptor==NED_VELOCITY_GPS_DATA_DESCRIPTOR))
          {
          is3dmgx335_extract_ned_velocity(field,&(pData->north),&(pData->east),&(pData->down),&(pData->speed),&(pData->groundSpeed),
                                          &(pData->heading),&(pData->speedAccuracy),&(pData->headingAccuracy),&(pData->nedVelocityValidFlags));
//             pdebug("N: %6.2f, E: %6.2f, D: %6.2f, S: %6.2f\n",pData->north,pData->east,pData->down,pData->speed);
//             pdebug("GS: %6.2f, H: %6.2f, SA: %6.2f, HA: %6.2f V: 0x%02hX\n",pData->groundSpeed,pData->heading,pData->speedAccuracy,pData->headingAccuracy,validFlags);
          }
      else
          {
          pdebug("Unknown descriptorSet: 0x%02hX and fieldDescriptor: 0x%02hX\n",descriptorSet,fieldDescriptor);
          }
       }
    }
 return(0);
}


//---------------------------------------------------------------------------
//
// void init_is3dmgx335_data(IS3DMGX335Data* pIs3dmgx335Data)
//
//---------------------------------------------------------------------------
void init_is3dmgx335_data(IS3DMGX335Data* pIs3dmgx335Data)
{
 pIs3dmgx335Data->xAccel=-0.17;
 pIs3dmgx335Data->yAccel=0.13;
 pIs3dmgx335Data->zAccel=1.414;
 pIs3dmgx335Data->xGyro=-0.17;
 pIs3dmgx335Data->yGyro=0.13;
 pIs3dmgx335Data->zGyro=1.414;
 pIs3dmgx335Data->xMag=-0.17;
 pIs3dmgx335Data->yMag=0.13;
 pIs3dmgx335Data->zMag=1.414;
int i,j;
  for(i=0;i<3;i++)
     {
     for(j=0;j<3;j++)
        {
        if(i==j)
           {
           pIs3dmgx335Data->orientationMatrix[i][j]=1.0;
           }
        else
           {
           pIs3dmgx335Data->orientationMatrix[i][j]=0.0;
           }
        }
     }
 pIs3dmgx335Data->yaw=-17.0;
 pIs3dmgx335Data->pitch=-1.3;
 pIs3dmgx335Data->roll=1.732;
 pIs3dmgx335Data->latitude=44.416668;
 pIs3dmgx335Data->longitude=8.850000;
 pIs3dmgx335Data->heightAboveEllipsode=66.6;
 pIs3dmgx335Data->heightAboveMSL=77.7;
 pIs3dmgx335Data->horizontalAccuracy=1.11;
 pIs3dmgx335Data->verticalAccuracy=2.22;
 pIs3dmgx335Data->llhPositionValidFlags=0x0000;
 pIs3dmgx335Data->north=3.141;
 pIs3dmgx335Data->east=2.781;
 pIs3dmgx335Data->down=3.1415;
 pIs3dmgx335Data->speed=-0.88;
 pIs3dmgx335Data->groundSpeed=9.99;
 pIs3dmgx335Data->heading=3.33;
 pIs3dmgx335Data->speedAccuracy=44.4;
 pIs3dmgx335Data->headingAccuracy=5.5;
 pIs3dmgx335Data->nedVelocityValidFlags=0x0000;
}


//---------------------------------------------------------------------------
//
// void print_date_and_time(void)
//
//---------------------------------------------------------------------------
void print_date_and_time(void)
{
 printf("%s %s",__DATE__,__TIME__);
}


//---------------------------------------------------------------------------
//
// double module_180(double angle)
//
//---------------------------------------------------------------------------
double module_180(double angle)
{
 while(angle<=-180.0)
    {
    angle+=360.0;
    }
 while(angle>180.0)
    {
    angle-=360.0;
    }
 return(angle);
}


//---------------------------------------------------------------------------
//
// void input_output_channels_init(int digitalInput[EUROPE_DIO_MAX_CHANNELS],
//                                 int digitalOutput[EUROPE_DIO_MAX_CHANNELS],
//                                 int digitalOutputPulseCounter[EUROPE_DIO_MAX_CHANNELS],
//                                 double analogInput[EUROPE_AD_MAX_CHANNELS],
//                                 double analogOutput[EUROPE_DA_MAX_CHANNELS])
//
//---------------------------------------------------------------------------
void input_output_channels_init(int digitalInput[EUROPE_DIO_MAX_CHANNELS],
                                int digitalOutput[EUROPE_DIO_MAX_CHANNELS],
                                int digitalOutputPulseCounter[EUROPE_DIO_MAX_CHANNELS],
                                double analogInput[EUROPE_AD_MAX_CHANNELS],
                                double analogOutput[EUROPE_DA_MAX_CHANNELS])
{
int i;
 for(i=0;i<EUROPE_DIO_MAX_CHANNELS;i++)
    {
    digitalOutput[i]=digitalInput[i]=digitalOutputPulseCounter[i]=0;
    }
 for(i=0;i<EUROPE_AD_MAX_CHANNELS;i++)
    {
    analogInput[i]=0.0;
    }
 for(i=0;i<EUROPE_DA_MAX_CHANNELS;i++)
    {
    analogOutput[i]=0.0;
    }
// digitalOutput[EUROPE_DIO_CTD_POWER]=1;
 digitalOutput[EUROPE_DIO_PA500_1_POWER]=1;
}


//---------------------------------------------------------------------------
//
// void io_boards_init(Diamond* pDiamond0,Diamond* pDiamond1)
//
//---------------------------------------------------------------------------
void io_boards_init(Diamond* pDiamond0,Diamond* pDiamond1)
{

int diamond0PortType[DIAMOND_DIO_MAX_PORTS];
int diamond0NegatedBit[DIAMOND_DIO_MAX_CHANNELS];
int diamond1PortType[DIAMOND_DIO_MAX_PORTS];
int diamond1NegatedBit[DIAMOND_DIO_MAX_CHANNELS];
int i;

 diamond0PortType[0]=0; // output
 diamond0PortType[1]=0; // output
 diamond0PortType[2]=0; // output
 diamond0PortType[3]=0; // output

 diamond1PortType[0]=0; // output
 diamond1PortType[1]=1; // input
 diamond1PortType[2]=1; // input
 diamond1PortType[3]=0; // output

 for(i=0;i<DIAMOND_DIO_MAX_CHANNELS;i++)
    {
    diamond0NegatedBit[i]=0;
    diamond1NegatedBit[i]=0;
    }

 diamond_init(pDiamond0,DIAMOND_0_BASE_ADDRESS,diamond0PortType,
              diamond0NegatedBit);
 diamond_init(pDiamond1,DIAMOND_1_BASE_ADDRESS,diamond1PortType,
              diamond1NegatedBit);
}


//---------------------------------------------------------------------------
//
// void io_boards_terminate(Diamond* pDiamond0,Diamond* pDiamond1)
//
//---------------------------------------------------------------------------
void io_boards_terminate(Diamond* pDiamond0,Diamond* pDiamond1)
{
 diamond_terminate(pDiamond0);
 diamond_terminate(pDiamond1);
}


//---------------------------------------------------------------------------
//
// void io_boards_get_digital_inputs(Diamond* pDiamond0,Diamond* pDiamond1,
//                                   int digitalInput[EUROPE_DIO_MAX_CHANNELS])
//
//---------------------------------------------------------------------------
void io_boards_get_digital_inputs(Diamond* pDiamond0,Diamond* pDiamond1,
                                  int digitalInput[EUROPE_DIO_MAX_CHANNELS])
{
int i;
int bit[DIAMOND_DIO_MAX_CHANNELS];

 diamond_di_get(pDiamond0,bit);
 for(i=0;i<DIAMOND_DIO_MAX_CHANNELS;i++)
    {
    digitalInput[i]=bit[i];
    }
 diamond_di_get(pDiamond1,bit);
 for(i=0;i<DIAMOND_DIO_MAX_CHANNELS;i++)
    {
    digitalInput[i+DIAMOND_DIO_MAX_CHANNELS]=bit[i];
    }
}


//---------------------------------------------------------------------------
//
// void io_boards_put_digital_outputs(Diamond* pDiamond0,Diamond* pDiamond1,
//                                    int digitalOutput[EUROPE_DIO_MAX_CHANNELS])
//
//---------------------------------------------------------------------------
void io_boards_put_digital_outputs(Diamond* pDiamond0,Diamond* pDiamond1,
                                   int digitalOutput[EUROPE_DIO_MAX_CHANNELS])
{
int i;
int bit[DIAMOND_DIO_MAX_CHANNELS];
 for(i=0;i<DIAMOND_DIO_MAX_CHANNELS;i++)
    {
    bit[i]=digitalOutput[i];
    }
 diamond_do_put(pDiamond0,bit);
 for(i=0;i<DIAMOND_DIO_MAX_CHANNELS;i++)
    {
    bit[i]=digitalOutput[i+DIAMOND_DIO_MAX_CHANNELS];
    }
 diamond_do_put(pDiamond1,bit);
}


//---------------------------------------------------------------------------
//
// void io_boards_get_analog_inputs(Diamond* pDiamond0,Diamond* pDiamond1,
//                                  double analogInput[EUROPE_AD_MAX_CHANNELS])
//
//---------------------------------------------------------------------------
void io_boards_get_analog_inputs(Diamond* pDiamond0,Diamond* pDiamond1,
                                 double analogInput[EUROPE_AD_MAX_CHANNELS])
{
int i;
double voltage[DIAMOND_AD_MAX_CHANNELS];

 diamond_ad_get(pDiamond0,voltage);
 for(i=0;i<DIAMOND_AD_MAX_CHANNELS;i++)
    {
    analogInput[i]=voltage[i];
    }
 analogInput[EUROPE_AD_INTERNAL_TEMPERATURE]=-voltage[EUROPE_AD_INTERNAL_TEMPERATURE];
 diamond_ad_get(pDiamond1,voltage);
 for(i=0;i<DIAMOND_AD_MAX_CHANNELS;i++)
    {
    analogInput[i+DIAMOND_AD_MAX_CHANNELS]=voltage[i];
    }
}


//---------------------------------------------------------------------------
//
// void io_boards_put_analog_outputs(Diamond* pDiamond0,Diamond* pDiamond1,
//                                   double analogOutput[EUROPE_DA_MAX_CHANNELS])
//
//---------------------------------------------------------------------------
void io_boards_put_analog_outputs(Diamond* pDiamond0,Diamond* pDiamond1,
                                  double analogOutput[EUROPE_DA_MAX_CHANNELS])
{
int i;
double voltage[DIAMOND_DA_MAX_CHANNELS];

 for(i=0;i<DIAMOND_DA_MAX_CHANNELS;i++)
    {
    voltage[i]=analogOutput[i];
    }
 voltage[0]=-voltage[0];
 voltage[1]=voltage[1];
 voltage[2]=voltage[2];
 voltage[3]=-voltage[3];

 diamond_da_put(pDiamond0,voltage);

 for(i=0;i<DIAMOND_DA_MAX_CHANNELS;i++)
    {
    voltage[i]=analogOutput[i+DIAMOND_DA_MAX_CHANNELS];
    }
 voltage[0]=-voltage[0];
 voltage[1]=+voltage[1];
 voltage[2]=-voltage[2];
 voltage[3]=+voltage[3];

 diamond_da_put(pDiamond1,voltage);
}
