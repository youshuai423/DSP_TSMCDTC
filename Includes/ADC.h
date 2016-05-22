#ifndef ADC_H_
#define ADC_H_
/******************************************************************************
| includes
|--------------------------------------------------------------------------------------------*/
#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

/******************************************************************************
| macros
|--------------------------------------------------------------------------------------------*/
#define Nospl				4  // 采样个数，需为偶数
#define sRange				5  // 采样最大电压

#define AD7606_SCK_LOW					GpioDataRegs.GPACLEAR.bit.GPIO25=1;
#define AD7606_SCK_HIGH					GpioDataRegs.GPASET.bit.GPIO25=1;

#define AD7606_RST_LOW				    GpioDataRegs.GPACLEAR.bit.GPIO27=1;
#define AD7606_RST_HIGH					GpioDataRegs.GPASET.bit.GPIO27=1;

#define AD7606_CNVST_LOW				GpioDataRegs.GPACLEAR.bit.GPIO24=1;
#define AD7606_CNVST_HIGH				GpioDataRegs.GPASET.bit.GPIO24=1;

#define AD7606_SCS_LOW				    GpioDataRegs.GPBCLEAR.bit.GPIO61=1;
#define AD7606_SCS_HIGH					GpioDataRegs.GPBSET.bit.GPIO61=1;

#define AD7606_BUSY_READ                GpioDataRegs.GPADAT.bit.GPIO26
#define AD7606_DOUTA_READ               GpioDataRegs.GPBDAT.bit.GPIO60
#define AD7606_DOUTB_READ               GpioDataRegs.GPBDAT.bit.GPIO32

/******************************************************************************
| variables
|--------------------------------------------------------------------------------------------*/
/*
extern float CH1_Sample[256];
extern float CH2_Sample[256];
extern float CH3_Sample[256];
extern float CH4_Sample[256];
extern float CH5_Sample[256];
extern float CH6_Sample[256];
extern float CH7_Sample[256];
extern float CH8_Sample[256];
extern Uint16 adc_cnt;  // adc存储下标 */
extern int conv_flg;  // 转换完成标志

/******************************************************************************
| local functions prototypes
|--------------------------------------------------------------------------------------------*/
void ADCRDOneSpl(double * buf);
void ADCRST(void);
void ADCInit(void);

#endif /* ADC_H_ */
