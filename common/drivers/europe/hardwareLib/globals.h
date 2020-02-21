/*=========================================================================*\
||                                                                         ||
||                  Code written by: Gabriele Bruzzone                     ||
||                                                                         ||
\*=========================================================================*/

// Release 1.7, 10/09/2014, added decoding of other is3dmgx335 data messages
// Release 1.6, 19/08/2014, added pa500
// Release 1.5, 30/07/2014, added is3dmgx335
// Release 1.4, 21/07/2014, added ctd_305_plus
// Release 1.3, 09/07/2014, added ctd_304_plus
// Release 1.2, 26/03/2014, added ctd_idromar
// Release 1.1, 12/03/2014, added diamond
// Release 1.0, 27/07/2012

//---------------------------------------------------------------------------
//
// globals.h
//
//---------------------------------------------------------------------------

#ifndef GLOBALS_H
#define GLOBALS_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <netinet/in.h>
#include <stdio.h>
#include "../../../generic/define.h"

#define _PDEBUG_STRING_SIZE (1024)
#ifdef __MAIN__
//char _debugString[_PDEBUG_STRING_SIZE];
#else
//extern char _debugString[_PDEBUG_STRING_SIZE];
#endif // __MAIN__

#define pdebug(fmt,args...)\
        {sprintf(_debugString,fmt,##args);\
        fprintf(stdout,"%s:%d - %s",__FILE__,__LINE__,_debugString);\
        fflush(stdout);}

typedef struct
   {
   int fdSio;
   }RadioModem;

int radio_modem_init(RadioModem* pRadioModem,const char* device);
int radio_modem_enable(RadioModem* pRadioModem,int enable);
int radio_modem_terminate(RadioModem* pRadioModem);
void radio_modem_send(RadioModem* pRadioModem,const char* buffer,int bufferLength);
int radio_modem_recv(RadioModem* pRadioModem,char* buffer,int* pBufferLength);
int radio_modem_build_message(char* message,const char* body,int bodyLength);
int radio_modem_extract_body(char inChar,char* body,int* pBodyLength);

int timer_init(long periodMs);
int timer_start(void);
int timer_wait_sync(void);
int timer_terminate(void);

int serial_init(const char* device,int baudRate);
int serial_terminate(int fd);

/*
typedef struct
   {
   int socketId;
   struct sockaddr_in peerSockAddr;
   }Datagram;

int datagram_init(Datagram* pDatagram,int16_t localUdpPort,
                  const char* peerIpAddress,int16_t peerUdpPort);
int datagram_send(Datagram* pDatagram,const char* buffer,int bufferLength);
int datagram_recv(Datagram* pDatagram,char* buffer,int* pBufferLength);
int datagram_terminate(Datagram* pDatagram);
*/

typedef struct
   {
   int fd;
   char name[256];
   int maxBufferSize;
   char* buffer;
   int bufferSize;
   }CachedLogBuffer;

int cached_log_buffer_init(CachedLogBuffer* pCachedLogBuffer,int size);
int cached_log_buffer_open(CachedLogBuffer* pCachedLogBuffer,const char* path,const char* name);
int cached_log_buffer_close(CachedLogBuffer* pCachedLogBuffer);
int cached_log_buffer_write(CachedLogBuffer* pCachedLogBuffer,const char* buffer,int bufferLength);
int cached_log_buffer_terminate(CachedLogBuffer* pCachedLogBuffer);

void print_date_and_time(void);

typedef struct
   {
   int numSamples;
   int windowLength;
   int index;
   double* buffer;
   double sum;
   double oldValue;
   }LowPassFilter;

int low_pass_filter_init(LowPassFilter* pLowPassFilter,int windowLength);
void low_pass_filter_terminate(LowPassFilter* pLowPassFilter);
double low_pass_filter_update(LowPassFilter* pLowPassFilter,double value);

typedef struct
   {
   int fdSio;
   }GPSNMEA;

int gps_nmea_init(GPSNMEA* pGPSNMEA,const char* device);
int gps_nmea_terminate(GPSNMEA* pGPSNMEA);
int gps_nmea_receive_sentence(GPSNMEA* pGPSNMEA,char* sentence,
                              int sentenceMaxLength,char* sentenceType);

double module_180(double angle);
double saturate(double x,double a,double b);

typedef struct
   {
   int size;
   char* data;
   }FIFOElement;

typedef struct
   {
   int maxSize;
   int maxElementSize;
   int head;
   int tail;
   int numElements;
   FIFOElement *element;
   }FIFO;

int fifo_init(FIFO *pFIFO,int maxSize,int maxElementSize);
int fifo_terminate(FIFO *pFIFO);
int fifo_put(FIFO *pFIFO,const char *element,int elementSize);
int fifo_get(FIFO *pFIFO,char *element,int *pElementSize);
int fifo_peek_first(FIFO *pFIFO,char *element,int *pElementSize);
int fifo_peek_last(FIFO *pFIFO,char *element,int *pElementSize);
int fifo_peek_element(FIFO *pFIFO,char *element,int *pElementSize,int i);
int fifo_get_head_index(FIFO *pFIFO);
int fifo_get_tail_index(FIFO *pFIFO);
int fifo_reset(FIFO *pFIFO);

typedef struct
   {
   int fd;
   int type;
   }AcousticModem;

typedef enum{
   SERIAL_ACOUSTIC_MODEM_TYPE  =0,
   ETHERNET_ACOUSTIC_MODEM_TYPE=1
   }AcousticModemType;

typedef struct
   {
   int seconds;
   int address;
   double x;
   double y;
   double z;
   }AcousticModemPosition;

int acoustic_modem_init(AcousticModem* pAcousticModem,int type,
                        const char* serialDeviceOrIpAddress,uint16_t tcpPort);
int acoustic_modem_terminate(AcousticModem* pAcousticModem);
void acoustic_modem_iper_fsm(AcousticModem *pAcousticModem,
                             FIFO *pCommandFIFO,
                             FIFO *pTelemetryFIFO,
                             FIFO *pPositionFIFO,
                             int getPositionPeriodInTenthsOfSecond,
                             int tenthsOfSecondsPerTick);
void acoustic_modem_sub_fsm(AcousticModem *pAcousticModem,
                            FIFO *pCommandFIFO,
                            FIFO *pTelemetryFIFO);
int acoustic_modem_init(AcousticModem* pAcousticModem,int type,
                        const char* serialDeviceOrIpAddress,uint16_t tcpPort);

#define DIAMOND_AD_MAX_CHANNELS               (16)
#define DIAMOND_DA_MAX_CHANNELS                (4)
#define DIAMOND_DIO_MAX_CHANNELS              (24)
#define DIAMOND_DIO_MAX_PORTS                  (4)

typedef struct
   {
   ushort baseAddress;
   ushort rADStart;
   ushort rADLSB;
   ushort rADMSB;
   ushort rADLowChannel;
   ushort rADHighChannel;
   ushort rDALSB12;
   ushort rDAMSB12;
   ushort rADFIFODepth;
   ushort rADFIFOControl;
   ushort rMiscAndPageControl;
   ushort rADStatus;
   ushort rADConfig;
   ushort rDIOPortA;
   ushort rDIOPortB;
   ushort rDIOPortC;
   ushort rDIOControl;
   ushort rEnhancedFeatures;
   ushort rDALSB16;
   ushort rDAMSB16;
   ushort rDIOConfiguration;
   int rDIOPortType[DIAMOND_DIO_MAX_PORTS];
   int negatedBit[DIAMOND_DIO_MAX_CHANNELS];
   }Diamond;

int diamond_init(Diamond* pDiamond,ushort baseAddress,int portType[DIAMOND_DIO_MAX_PORTS],
                 int negatedBit[DIAMOND_DIO_MAX_CHANNELS]);
void diamond_reset(Diamond* pDiamond);
int diamond_ad_get(Diamond* pDiamond,double voltage[DIAMOND_AD_MAX_CHANNELS]);
int diamond_da_put(Diamond* pDiamond,double voltage[DIAMOND_DA_MAX_CHANNELS]);
int diamond_di_get(Diamond* pDiamond,int bit[DIAMOND_DIO_MAX_CHANNELS]);
int diamond_do_put(Diamond* pDiamond,const int bit[DIAMOND_DIO_MAX_CHANNELS]);
int diamond_dio_is_input(Diamond* pDiamond,int ch);
int diamond_terminate(Diamond* pDiamond);

typedef struct
   {
   int fdSio;
   }CtdIdromar;

int ctd_idromar_init(CtdIdromar* pCtdIdromar,const char* device);
int ctd_idromar_terminate(CtdIdromar* pCtdIdromar);
int ctd_idromar_read_depth_100(CtdIdromar* pCtdIdromar,double* pDepth100);
int ctd_idromar_read_depth_1000(CtdIdromar* pCtdIdromar,double* pDepth1000);
int ctd_idromar_read_temperature(CtdIdromar* pCtdIdromar,double* pTemperature);
int ctd_idromar_read_conductivity(CtdIdromar* pCtdIdromar,double* pConductivity);

typedef struct
   {
   int fdSio;
   }Ctd304Plus;

int ctd_304_plus_init(Ctd304Plus* pCtd304Plus,const char* device);
int ctd_304_plus_terminate(Ctd304Plus* pCtd304Plus);
int ctd_304_plus_read(Ctd304Plus* pCtd304Plus,double* pConductivity,
                      double* pTemperature,double* pDepth);

typedef struct
   {
   int fdSio;
   }Ctd305Plus;

int ctd_305_plus_init(Ctd305Plus* pCtd305Plus,const char* device);
int ctd_305_plus_terminate(Ctd305Plus* pCtd305Plus);
int ctd_305_plus_read(Ctd305Plus* pCtd305Plus,double* pConductivity,
                      double* pTemperature,double* pDepth);

typedef struct
   {
   int fdSio;
   }IS3DMGX335;

   typedef struct
      {
      double xAccel;
      double yAccel;
      double zAccel;
      double xGyro;
      double yGyro;
      double zGyro;
      double xMag;
      double yMag;
      double zMag;
      double orientationMatrix[3][3];
      double yaw;
      double pitch;
      double roll;
      double latitude;
      double longitude;
      double heightAboveEllipsode;
      double heightAboveMSL;
      double horizontalAccuracy;
      double verticalAccuracy;
      uint16_t llhPositionValidFlags;
      double north;
      double east;
      double down;
      double speed;
      double groundSpeed;
      double heading;
      double speedAccuracy;
      double headingAccuracy;
      uint16_t nedVelocityValidFlags;
      }IS3DMGX335Data;

int is3dmgx335_init(IS3DMGX335* pIS3DMGX335,const char* device);
int is3dmgx335_terminate(IS3DMGX335* pIS3DMGX335);
int is3dmgx335_read(IS3DMGX335* pIS3DMGX335,IS3DMGX335Data* pData);

typedef struct
   {
   int fdSio;
   int mode;
   }PA500;

typedef enum{
   FREE_RUNNING_PA500_MODE   =0,
   INTERROGATE_PA500_MODE    =1
   }PA500Mode;

int pa500_init(PA500* pPA500,const char* device,int mode);
int pa500_terminate(PA500* pPA500);
int pa500_read(PA500* pPA500,double* pRange);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // GLOBALS_H
