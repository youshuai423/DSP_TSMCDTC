/******************************************************************************
| includes
|--------------------------------------------------------------------------------------------*/
#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

/******************************************************************************
| defines
|--------------------------------------------------------------------------------------------*/
//#define period				7500  // 10KHz对应时钟数，TBCLK = SYSCLKOUT
#define period				15000  // 10KHz对应时钟数，TBCLK = SYSCLKOUT(for test)
#define prediv				0  // 预分频
//#define limitclk				150  // 预分频后对应的1us时钟数
#define limitclk				750  // 预分频后对应的1us时钟数
#define zerolimit				300
#define DT				150   // 死区

/******************************************************************************
| variables
|--------------------------------------------------------------------------------------------*/

/******************************************************************************
| local functions prototypes
|--------------------------------------------------------------------------------------------*/
extern void InitPWM(void);
