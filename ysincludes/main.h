/******************************************************************************
| includes
|----------------------------------------------------------------------------*/
#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "ysPWM.h"
#include "ysPORT.h"
#include "ysADC.h"
#include "imcontrol.h"

/******************************************************************************
| defines
|----------------------------------------------------------------------------*/
#define pi 3.1415926
#define digit				100000  // 用于四舍五入
#define HallRatioV1				135.5
#define HallRatioV2				135.0
#define HallRatioC				0.74

/******************************************************************************
| variables
|----------------------------------------------------------------------------*/

/******************************************************************************
| local functions prototypes
|----------------------------------------------------------------------------*/
interrupt void epwm1_timer_isr(void);
int sign(double);  // sign函数
