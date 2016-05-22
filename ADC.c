/******************************************************************************
| includes
|-------------------------------------------------------------------------------------------*/
#include "ADC.h"

/******************************************************************************
| variables
|--------------------------------------------------------------------------------------------*/
int conv_flg = 0;

/******************************************************************************
| functions
|-------------------------------------------------------------------------------------------*/
void ADCRDOneSpl(double * buf)
{
	unsigned char j, k;
	unsigned short int TempA, TempB;

   /* 转换开始 */
	AD7606_CNVST_LOW;
	DELAY_US(1);  // *
	AD7606_CNVST_HIGH;
	DELAY_US(1);

	while(AD7606_BUSY_READ==1)
	{
	}

	 /* 片选信号有效 */
	AD7606_SCS_LOW;

	for(j=0; j<Nospl / 2; j++)
	{
		TempA=0;
		TempB=0;

		for(k=0; k<16; k++)
		{
			AD7606_SCK_LOW;

			TempA=(TempA<<1) + AD7606_DOUTA_READ;
			TempB=(TempB<<1) + AD7606_DOUTB_READ;

			AD7606_SCK_HIGH;
		}

		buf[2 * j]=(int)TempA * (sRange * 2 / 65536.0);
		buf[2 * j + 1]=(int)TempB * (sRange * 2 / 65536.0); //数字量转换为模拟量,输入范围是正负10V,精度为16位
		                                //相当于将20V分成了65536份,公式为A=(20.0/65536.0)*D;A为模拟量值，D为数字量值;
		                                //如果输入范围是正负5V则公式为A=(10.0/65536.0)*D
	}
	AD7606_SCS_HIGH;
	conv_flg=1;
}

void ADCRST(void)
{
	AD7606_RST_HIGH;
	DELAY_US(1000);
	AD7606_RST_LOW;
	DELAY_US(1000);
}

void ADCInit(void)
{
   EALLOW;
   GpioCtrlRegs.GPAMUX2.bit.GPIO24 = 0; // GPIO0 = GPIO0
   GpioCtrlRegs.GPADIR.bit.GPIO24 = 1;
   GpioCtrlRegs.GPAMUX2.bit.GPIO25 = 0; // GPIO1 = GPIO1
   GpioCtrlRegs.GPADIR.bit.GPIO25 = 1;
   GpioCtrlRegs.GPAMUX2.bit.GPIO27 = 0; // GPIO2 = GPIO2
   GpioCtrlRegs.GPADIR.bit.GPIO27 = 1;
   GpioCtrlRegs.GPBMUX2.bit.GPIO61 = 0; // GPIO3 = GPIO3
   GpioCtrlRegs.GPBDIR.bit.GPIO61 = 1;
   GpioCtrlRegs.GPAMUX2.bit.GPIO26 = 0; // GPIO4 = GPIO4
   GpioCtrlRegs.GPADIR.bit.GPIO26 = 0;
   GpioCtrlRegs.GPBMUX2.bit.GPIO60 = 0; // GPIO5 = GPIO5
   GpioCtrlRegs.GPBDIR.bit.GPIO60 = 0;
   GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 0; // GPIO5 = GPIO5
   GpioCtrlRegs.GPBDIR.bit.GPIO32 = 0;

   EDIS;
   AD7606_SCK_HIGH;
   AD7606_RST_HIGH;
   AD7606_CNVST_HIGH;
   AD7606_SCS_HIGH;
}
