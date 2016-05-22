#ifndef EPWM_H_
#define EPWM_H_

/******************************************************************************
| includes
|--------------------------------------------------------------------------------------------*/
#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "math.h"
#include "ADC.h"

/******************************************************************************
| macros
|--------------------------------------------------------------------------------------------*/
#define period				15000  // 10KHz对应时钟数，TBCLK = SYSCLKOUT
//#define period 37500  // 10KHz对应时钟数，TBCLK = SYSCLKOUT(for test)
#define M				0.9  // 调制度
#define DT				150  // 死区
#define digit				100000
#define HallRatioV1				135.5
#define HallRatioV2				135
#define HallRatioC				0.74

#define PWM1U_HIGH				EPwm1Regs.AQSFRC.bit.ACTSFA = AQ_SET;
#define PWM1U_LOW				EPwm1Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
#define PWM1L_HIGH				EPwm1Regs.AQSFRC.bit.ACTSFB = AQ_SET;
#define PWM1L_LOW				EPwm1Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;

#define PWM2U_HIGH				EPwm2Regs.AQSFRC.bit.ACTSFA = AQ_SET;
#define PWM2U_LOW				EPwm2Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
#define PWM2L_HIGH				EPwm2Regs.AQSFRC.bit.ACTSFB = AQ_SET;
#define PWM2L_LOW				EPwm2Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;

#define PWM3U_HIGH				EPwm3Regs.AQSFRC.bit.ACTSFA = AQ_SET;
#define PWM3U_LOW				EPwm3Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
#define PWM3L_HIGH				EPwm3Regs.AQSFRC.bit.ACTSFB = AQ_SET;
#define PWM3L_LOW				EPwm3Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;

#define PWM1U_DC				EPwm1Regs.CMPA.half.CMPA
#define PWM1L_DC				EPwm1Regs.CMPB

#define PWM2U_DC				EPwm2Regs.CMPA.half.CMPA
#define PWM2L_DC				EPwm2Regs.CMPB

#define PWM3U_DC				EPwm3Regs.CMPA.half.CMPA
#define PWM3L_DC				EPwm3Regs.CMPB

/******************************************************************************
| variables
|--------------------------------------------------------------------------------------------*/
extern double mSample[4];
extern double Ua, Ub, Uc;
extern double ia, ib, ic;
extern unsigned int dutycycle;
extern unsigned int sector;

/******************************************************************************
| local functions prototypes
|--------------------------------------------------------------------------------------------*/
void ePWMInit(void);
interrupt void epwm1_timer_isr(void);
double roundn(double);  // 截断小数点后位数
int sign(double);  // sign函数

#endif /* EPWM_H_ */
