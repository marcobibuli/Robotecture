/*=========================================================================*\
||                                                                         ||
||                  Code written by: Gabriele Bruzzone                     ||
||                                                                         ||
\*=========================================================================*/

// Release 1.0, 25/03/2016

//---------------------------------------------------------------------------
//
// e-URoPelib.h
//
//---------------------------------------------------------------------------

#ifndef EUROPELIB_H
#define EUROPELIB_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "definitions.h"
#include "globals.h"


typedef struct
   {
   int isSimulated;
   int workingMode;
   int controlMode;
   char hciIPAddress[32];
   char is3dmgx335SerialDeviceName[32];
   char ctd304SerialDeviceName[32];
   char pa500_1SerialDeviceName[32];
   char pa500_2SerialDeviceName[32];
   }Parameters;

typedef enum
   {
   DEBUG_WORKING_MODE     = 0,
   NORMAL_WORKING_MODE    = 1
   }WorkingModesEnum;

typedef enum
   {
   DEPTH_MODE       = 0,
   ALTITUDE_MODE    = 1
   }DepthAltitudeModesEnum;

typedef enum
   {
   DEBUG_PLANT_CONTROL_MODE      = -1,
   DEBUG_MANUAL_CONTROL_MODE     =  0,
   DEBUG_AUTOMATIC_CONTROL_MODE  =  1
   }DebugControlModesEnum;

#define PROMPT_STRING_SIZE  (64)
#define INPUT_STRING_SIZE   (64)
#define ERROR_STRING_SIZE   (64)

//extern volatile int endLoop;



int diamond_init(Diamond* pDiamond,ushort baseAddress,int portType[DIAMOND_DIO_MAX_PORTS],
                 int negatedBit[DIAMOND_DIO_MAX_CHANNELS]);
void diamond_reset(Diamond* pDiamond);
int diamond_ad_get(Diamond* pDiamond,double voltage[DIAMOND_AD_MAX_CHANNELS]);
int diamond_da_put(Diamond* pDiamond,double voltage[DIAMOND_DA_MAX_CHANNELS]);
int diamond_di_get(Diamond* pDiamond,int bit[DIAMOND_DIO_MAX_CHANNELS]);
int diamond_do_put(Diamond* pDiamond,const int bit[DIAMOND_DIO_MAX_CHANNELS]);
int diamond_dio_is_input(Diamond* pDiamond,int ch);
int diamond_terminate(Diamond* pDiamond);


void print_lib_version(void);
int application_init(int workingMode,int controlMode);
int application_terminate(int workingMode,int controlMode);
int read_configuration_file(const char* fileName,Parameters* p);
void input_output_channels_init(int digitalInput[EUROPE_DIO_MAX_CHANNELS],
                                int digitalOutput[EUROPE_DIO_MAX_CHANNELS],
                                int digitalOutputPulseCounter[EUROPE_DIO_MAX_CHANNELS],
                                double analogInput[EUROPE_AD_MAX_CHANNELS],
                                double analogOutput[EUROPE_DA_MAX_CHANNELS]);
void io_boards_init(Diamond* pDiamond0,Diamond* pDiamond1);
void io_boards_terminate(Diamond* pDiamond0,Diamond* pDiamond1);
void io_boards_get_digital_inputs(Diamond* pDiamond0,Diamond* pDiamond1,
                                  int digitalInput[EUROPE_DIO_MAX_CHANNELS]);
void io_boards_put_digital_outputs(Diamond* pDiamond0,Diamond* pDiamond1,
                                  int digitalOutput[EUROPE_DIO_MAX_CHANNELS]);
void io_boards_get_analog_inputs(Diamond* pDiamond0,Diamond* pDiamond1,
                                 double analogInput[EUROPE_AD_MAX_CHANNELS]);
void io_boards_put_analog_outputs(Diamond* pDiamond0,Diamond* pDiamond1,
                                  double analogOutput[EUROPE_DA_MAX_CHANNELS]);
void init_text_console_strings(char* promptString,char* inputString,char* errorString);
void text_console_update_screen(int digitalInput[EUROPE_DIO_MAX_CHANNELS],
                                int digitalOutput[EUROPE_DIO_MAX_CHANNELS],
                                double analogInput[EUROPE_AD_MAX_CHANNELS],
                                double analogOutput[EUROPE_DA_MAX_CHANNELS],
                                const char* promptString,
                                const char* inputString,
                                const char* errorString);
int text_console_manage_user_input_key_fsm(char* promptString,
                                           char* inputString,
                                           char* errorString,
                                           int digitalOutput[EUROPE_DIO_MAX_CHANNELS],
                                           double analogOutput[EUROPE_DA_MAX_CHANNELS]);
void vehicle_stop(int digitalOutput[EUROPE_DIO_MAX_CHANNELS],double analogOutput[EUROPE_DA_MAX_CHANNELS]);
void build_debug_working_mode_telemetry(uint16_t debugTelemetry[DEBUG_MANUAL_TELEMETRY_SIZE_IN_UINT16],
                                        int digitalInput[EUROPE_DIO_MAX_CHANNELS],double analogInput[EUROPE_AD_MAX_CHANNELS],
                                        double analogOutput[EUROPE_DA_MAX_CHANNELS],VehicleStatus* pVS);
void manage_digital_pulse(int digitalOutputPulseCounter[EUROPE_DIO_MAX_CHANNELS],
                          int digitalOutput[EUROPE_DIO_MAX_CHANNELS]);
void manage_initial_lights_effect(int digitalOutput[EUROPE_DIO_MAX_CHANNELS]);
/*
int receive_commands(Datagram *pCommunicationDatagram,FIFO *pCommandQueue,
                     int timeOutInTicks,int *pIsCommunicationLost);
void send_telemetry(Datagram *pCommunicationDatagram,const char* telemetry,
                   int telemetryLength);
*/

void process_debug_working_mode_commands(FIFO *pCommandQueue,
                                         int digitalOutput[EUROPE_DIO_MAX_CHANNELS],
                                         int digitalOutputPulseCounter[EUROPE_DIO_MAX_CHANNELS],
                                         double analogOutput[EUROPE_DA_MAX_CHANNELS],
                                         VehicleStatus* pVS);
void init_vehicle_status(VehicleStatus* pVS);
void init_is3dmgx335_data(IS3DMGX335Data* pIs3dmgx335Data);
void copy_is3dmgx335_data_to_vehicle_status_data(VehicleStatus* pVS,
                                                 IS3DMGX335Data* pIs3dmgx335Data);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EUROPELIB_H
