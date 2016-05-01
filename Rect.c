#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "math.h"

#define pi 3.1415926
#define digit 100000
#define period 7500  // 10KHz对应时钟数，TBCLK = SYSCLKOUT
//#define period 60000  // 10KHz对应时钟数，TBCLK = SYSCLKOUT(for test)
#define M 0.9  // 调制度
#define DT 50  // 死区

void ePWMSetup(void);
double roundn(double);  // 截断小数点后位数
int sign(double);  // sign函数
interrupt void epwm1_timer_isr(void);

void main()
{
   InitSysCtrl();

   DINT;

   InitPieCtrl();

   IER = 0x0000;
   IFR = 0x0000;

   InitPieVectTable();

   EALLOW;
   PieVectTable.EPWM1_INT = &epwm1_timer_isr;  // ePWM1中断函数入口
   EDIS;

   ePWMSetup();
	
   IER |= M_INT3;  // enable ePWM1 CPU_interrupt
   PieCtrlRegs.PIEIER3.bit.INTx1 = 1;  // enable ePWM1 pie_interrupt

   EINT;   // 总中断 INTM 使能
   ERTM;   // Enable Global realtime interrupt DBGM

   int i;
   for(; ;)
   {
	   asm("          NOP");
	   for(i=1;i<=10;i++)
	   {}
   }

}

void ePWMSetup(void)
{

   EALLOW;

   GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1; // GPIO 初始化
   GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;
   GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;
   GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;
   GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1;
   GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 1;

   EDIS;

   // ----------------EPwm1---------------------
   EPwm1Regs.TBPRD = period;
   EPwm1Regs.TBPHS.half.TBPHS = 0;  // 时基周期寄存器
   EPwm1Regs.TBCTR = 0;  // 时基计数寄存器置零
   EPwm1Regs.TBCTL.bit.PHSDIR = TB_UP;
   EPwm1Regs.TBCTL.bit.CLKDIV = 0;
   EPwm1Regs.TBCTL.bit.HSPCLKDIV = 0;
   EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;
   EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;

   EPwm1Regs.CMPA.half.CMPA = period / 2; // duty_cycle = 0.5
   EPwm1Regs.CMPB = period / 2;
   EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
   EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
   EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
   EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

   EPwm1Regs.AQCTLA.bit.CAU = AQ_TOGGLE;
   EPwm1Regs.AQCTLB.bit.CBU = AQ_TOGGLE;

   EPwm1Regs.ETSEL.bit.INTEN = 1;  // 使能ePWM中断
   EPwm1Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;
   EPwm1Regs.ETPS.all = 0x01; // interrupt on first event
   EPwm1Regs.ETPS.bit.INTPRD = ET_1ST;

   // ----------------EPwm2---------------------
   EPwm2Regs.TBPRD = period;
   EPwm2Regs.TBPHS.half.TBPHS = 0;  // 时基周期寄存器
   EPwm2Regs.TBCTR = 0;  // 时基计数寄存器置零
   EPwm2Regs.TBCTL.bit.PHSDIR = TB_UP;
   EPwm2Regs.TBCTL.bit.CLKDIV = 0;
   EPwm2Regs.TBCTL.bit.HSPCLKDIV = 0;
   EPwm2Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;
   EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;

   EPwm2Regs.CMPA.half.CMPA = period / 2; // duty_cycle = 0.5
   EPwm2Regs.CMPB = period / 2;
   EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
   EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
   EPwm2Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
   EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

   EPwm2Regs.AQCTLA.bit.CAU = AQ_TOGGLE;
   EPwm2Regs.AQCTLB.bit.CBU = AQ_TOGGLE;

   // ----------------EPwm3---------------------
   EPwm3Regs.TBPRD = period;
   EPwm3Regs.TBPHS.half.TBPHS = 0;  // 时基周期寄存器
   EPwm3Regs.TBCTR = 0;  // 时基计数寄存器置零
   EPwm3Regs.TBCTL.bit.PHSDIR = TB_UP;
   EPwm3Regs.TBCTL.bit.CLKDIV = 0;
   EPwm3Regs.TBCTL.bit.HSPCLKDIV = 0;
   EPwm3Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;
   EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;

   EPwm3Regs.CMPA.half.CMPA = period / 2; // duty_cycle = 0.5
   EPwm3Regs.CMPB = period / 2
		   ;
   EPwm3Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
   EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
   EPwm3Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
   EPwm3Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

   EPwm3Regs.AQCTLA.bit.CAU = AQ_TOGGLE;
   EPwm3Regs.AQCTLB.bit.CBU = AQ_TOGGLE;
}

interrupt void epwm1_timer_isr(void)
{
	double Ua, Ub, Uc;
	int Sa, Sb, Sc;
	unsigned int dutycycle;
	unsigned int sector;

	Ua = 0;
	Ub = 0;
	Uc = -Ua - Ub;

	/* ====扇区判断====*/
	Sa = sign(Ua);
	Sb = sign(Ub);
	Sc = sign(Uc);

	if (Sa && !Sb && !Sc)
		sector = 1;
	else if (Sa && Sb && !Sc)
		sector = 2;
	else if (!Sa && Sb && !Sc)
		sector = 3;
	else if (!Sa && Sb && Sc)
		sector = 4;
	else if (!Sa &&! Sb && Sc)
		sector = 5;
	else
		sector = 6;

   	switch (sector)
   	{
       case 1:
       {
    	   dutycycle = floor(-Ub / Ua * period);

    	   EPwm1Regs.AQSFRC.bit.ACTSFA = AQ_SET;
    	   EPwm1Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;
    	   EPwm2Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
    	   EPwm2Regs.AQSFRC.bit.ACTSFB = AQ_SET;
    	   EPwm3Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
    	   EPwm3Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;

    	   EPwm1Regs.CMPA.half.CMPA = period + 1;
    	   EPwm1Regs.CMPB = period + 1;
    	   EPwm2Regs.CMPA.half.CMPA = period + 1;
    	   EPwm2Regs.CMPB = dutycycle;
    	   EPwm3Regs.CMPA.half.CMPA = period + 1;
    	   EPwm3Regs.CMPB = period - dutycycle + 1;  // 防止两相短路
    	   break;
       }

        case 2:
        {
     	   dutycycle = floor(-Ub / Uc * period);

     	   EPwm1Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
     	   EPwm1Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;
     	   EPwm2Regs.AQSFRC.bit.ACTSFA = AQ_SET;
     	   EPwm2Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;
     	   EPwm3Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
     	   EPwm3Regs.AQSFRC.bit.ACTSFB = AQ_SET;

     	   EPwm1Regs.CMPA.half.CMPA = period - dutycycle + 1;
     	   EPwm1Regs.CMPB = period + 1;
     	   EPwm2Regs.CMPA.half.CMPA = dutycycle;
     	   EPwm2Regs.CMPB = period + 1;
     	   EPwm3Regs.CMPA.half.CMPA = period + 1;
     	   EPwm3Regs.CMPB = period + 1;
     	   break;
        }

        case 3:
        {
     	   dutycycle = floor(-Uc / Ub * period);

     	   EPwm1Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
     	   EPwm1Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;
     	   EPwm2Regs.AQSFRC.bit.ACTSFA = AQ_SET;
     	   EPwm2Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;
     	   EPwm3Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
     	   EPwm3Regs.AQSFRC.bit.ACTSFB = AQ_SET;

     	   EPwm1Regs.CMPA.half.CMPA = period + 1;
     	   EPwm1Regs.CMPB = period - dutycycle + 1;
     	   EPwm2Regs.CMPA.half.CMPA = period + 1;
     	   EPwm2Regs.CMPB = period + 1;
     	   EPwm3Regs.CMPA.half.CMPA = period + 1;
     	   EPwm3Regs.CMPB = dutycycle;
     	   break;
        }
        case 4:
        {
     	   dutycycle = floor(-Uc / Ua * period);

     	   EPwm1Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
     	   EPwm1Regs.AQSFRC.bit.ACTSFB = AQ_SET;
     	   EPwm2Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
     	   EPwm2Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;
     	   EPwm3Regs.AQSFRC.bit.ACTSFA = AQ_SET;
     	   EPwm3Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;

     	   EPwm1Regs.CMPA.half.CMPA = period + 1;
     	   EPwm1Regs.CMPB = period + 1;
     	   EPwm2Regs.CMPA.half.CMPA = period - dutycycle + 1;
     	   EPwm2Regs.CMPB = period + 1;
     	   EPwm3Regs.CMPA.half.CMPA = dutycycle;
     	   EPwm3Regs.CMPB = period + 1;
     	   break;
        }
        case 5:
        {
     	   dutycycle = floor(-Ua / Uc * period);

     	   EPwm1Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
     	   EPwm1Regs.AQSFRC.bit.ACTSFB = AQ_SET;
     	   EPwm2Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
     	   EPwm2Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;
     	   EPwm3Regs.AQSFRC.bit.ACTSFA = AQ_SET;
     	   EPwm3Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;

     	   EPwm1Regs.CMPA.half.CMPA = period + 1;
     	   EPwm1Regs.CMPB = dutycycle;
     	   EPwm2Regs.CMPA.half.CMPA = period + 1;
     	   EPwm2Regs.CMPB = period - dutycycle + 1;
     	   EPwm3Regs.CMPA.half.CMPA = period + 1;
     	   EPwm3Regs.CMPB = period + 1;
     	   break;
        }
        case 6:
        {
     	   dutycycle = floor(-Ua / Ub * period);

     	   EPwm1Regs.AQSFRC.bit.ACTSFA = AQ_SET;
     	   EPwm1Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;
     	   EPwm2Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
     	   EPwm2Regs.AQSFRC.bit.ACTSFB = AQ_SET;
     	   EPwm3Regs.AQSFRC.bit.ACTSFA = AQ_CLEAR;
     	   EPwm3Regs.AQSFRC.bit.ACTSFB = AQ_CLEAR;

     	   EPwm1Regs.CMPA.half.CMPA = dutycycle;
     	   EPwm1Regs.CMPB = period + 1;
     	   EPwm2Regs.CMPA.half.CMPA = period + 1;
     	   EPwm2Regs.CMPB = period + 1;
     	   EPwm3Regs.CMPA.half.CMPA = period - dutycycle + 1;
     	   EPwm3Regs.CMPB = period + 1;
     	   break;
        }
   	}

   // Clear INT flag for this timer
    EPwm1Regs.ETCLR.bit.INT = 1;

   // Acknowledge this interrupt to receive more interrupts from group 3
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

double roundn(double input)
{
	double temp;
	temp = input * digit;
	temp = floor(temp);
	temp = temp / digit;
	return temp;
}

int sign(double param)
{
	if (param >= 0)
		return 1;
	else
		return 0;
}
